﻿using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Diagnostics.SymbolStore;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.Emit;
using ClrType = System.Type;

namespace Owen
{
    internal static class IL
    {
        public static void Generate(Program program, string output)
        {
            var assembly = AppDomain.CurrentDomain.DefineDynamicAssembly(new AssemblyName(Path.GetFileNameWithoutExtension(output)), AssemblyBuilderAccess.Save);
            assembly.SetCustomAttribute
            (
                new CustomAttributeBuilder
                (
                    typeof(DebuggableAttribute).GetConstructor
                    (
                        new[]
                        {
                            typeof(DebuggableAttribute.DebuggingModes)
                        }
                    ),
                    new object[]
                    {
                        DebuggableAttribute.DebuggingModes.DisableOptimizations |
                        DebuggableAttribute.DebuggingModes.Default
                    }
                )
            );

            var module = assembly.DefineDynamicModule("MainModule", Path.GetFileName(output), true);
            var functionToBuilder = new Dictionary<FunctionDeclaration, MethodBuilder>();
            foreach (var file in program.Files)
            {
                foreach (var function in file.Functions)
                {
                    var method = module.DefineGlobalMethod
                    (
                        function.Name.Value,
                        MethodAttributes.Assembly | MethodAttributes.Public | MethodAttributes.Static | MethodAttributes.HideBySig,
                        ClrTypeFrom(function.Output),
                        function.Input.Select(i => ClrTypeFrom(i.Type)).ToArray()
                    );

                    functionToBuilder.Add(function, method);
                    if (method.Name == "main")
                        assembly.SetEntryPoint(method.GetBaseDefinition(), PEFileKinds.ConsoleApplication);
                }
            }

            foreach (var file in program.Files)
                Generate(file, functionToBuilder, module);

            module.CreateGlobalFunctions();

            var relative = Path.GetFileName(output);
            
            // In some cases AssemblyBuilder.Save(string) can cause an IOException
            // because the file at the relative path is being used in another
            // process. So far this have only been a problem when testing.
            // Deleting the file seems to fix the problem.
            if (System.IO.File.Exists(relative))
                System.IO.File.Delete(relative);

            assembly.Save(relative);
            if (Path.GetFullPath(relative) != output)
            {
                if (System.IO.File.Exists(output))
                    System.IO.File.Delete(output);

                System.IO.File.Move(relative, output);
            }
        }

        private static void Generate(File file, Dictionary<FunctionDeclaration, MethodBuilder> functionToBuilder, ModuleBuilder module)
        {
            var symbols = module.DefineDocument(file.Path, Guid.Empty, Guid.Empty, Guid.Empty);
            foreach (var function in file.Functions)
                Generate(function, functionToBuilder[function], functionToBuilder, symbols);
        }

        private static void Generate(FunctionDeclaration function, MethodBuilder method, Dictionary<FunctionDeclaration, MethodBuilder> functionToBuilder, ISymbolDocumentWriter symbols)
        {
            var instructions = method.GetILGenerator();

            Generate(function.Body, instructions, functionToBuilder, symbols);
            instructions.Emit(OpCodes.Ret);
        }

        private static void Generate(CompoundStatement body, ILGenerator instructions, Dictionary<FunctionDeclaration, MethodBuilder> functionToBuilder, ISymbolDocumentWriter symbols)
        {
            foreach (var statement in body.Statements)
            {
                if (statement is ExpressionStatement e)
                    Generate(e.Expression, instructions, functionToBuilder);
                else if (statement is ReturnStatement r)
                {
                    if (r.Expressions.Count == 1)
                        Generate(r.Expressions[0], instructions, functionToBuilder);
                    else if (r.Expressions.Count > 1)
                        Report.Error($"Cannot translate multiple return values to Clr.");

                    instructions.Emit(OpCodes.Ret);
                }
                else if (statement is AssertStatement assert)
                {
                    Generate(assert.Assertion, instructions, functionToBuilder);
                    instructions.Emit(OpCodes.Call, typeof(Debug).GetMethod("Assert", new[] { typeof(bool) }));
                }
                else
                    Report.Error($"Cannot translate {statement.GetType().Name} to IL.");
            }
        }

        private static void Generate(Expression expression, ILGenerator instructions, Dictionary<FunctionDeclaration, MethodBuilder> functionToBuilder)
        {
            if (expression is BinaryExpression binary)
            {
                Generate(binary.Left, instructions, functionToBuilder);
                Generate(binary.Right, instructions, functionToBuilder);

                switch (binary.Operator.Tag)
                {
                    case OperatorTag.EqualEqual:
                        instructions.Emit(OpCodes.Ceq);
                        break;
                    case OperatorTag.NotEqual:
                        instructions.Emit(OpCodes.Ceq);
                        instructions.Emit(OpCodes.Ldc_I4_0);
                        instructions.Emit(OpCodes.Ceq);
                        break;
                    default:
                        Report.Error($"{binary.Operator.DefinedAt} Cannot translate {binary.Operator.Tag} to IL.");
                        break;
                }
            }
            else if (expression is Call call)
            {
                if (call.Arguments.Count != 0)
                    Report.Error($"{call.Reference.StartsAt()} Cannot translate arguments to IL.");

                instructions.Emit(OpCodes.Call, functionToBuilder[call.Declaration]);
            }
            else if (expression is Number number)
            {
                switch (number.Tag)
                {
                    case NumberTag.I64:
                        instructions.Emit(OpCodes.Ldc_I8, long.Parse(number.Value));
                        break;
                    case NumberTag.I32:
                        instructions.Emit(OpCodes.Ldc_I4, int.Parse(number.Value));
                        break;
                    case NumberTag.I16:
                        instructions.Emit(OpCodes.Ldc_I4, short.Parse(number.Value));
                        break;
                    case NumberTag.I8:
                        instructions.Emit(OpCodes.Ldc_I4_S, sbyte.Parse(number.Value));
                        break;
                    case NumberTag.U64:
                        instructions.Emit(OpCodes.Ldc_I8, ulong.Parse(number.Value));
                        break;
                    case NumberTag.U32:
                        instructions.Emit(OpCodes.Ldc_I4, uint.Parse(number.Value));
                        break;
                    case NumberTag.U16:
                        instructions.Emit(OpCodes.Ldc_I4, ushort.Parse(number.Value));
                        break;
                    case NumberTag.U8:
                        instructions.Emit(OpCodes.Ldc_I4_S, byte.Parse(number.Value));
                        break;
                    case NumberTag.F32:
                        instructions.Emit(OpCodes.Ldc_R4, float.Parse(number.Value));
                        break;
                    case NumberTag.F64:
                        instructions.Emit(OpCodes.Ldc_R8, double.Parse(number.Value));
                        break;
                    default:
                        Report.Error($"Cannot translate {number.Tag} to IL.");
                        break;
                }
            }
            else if (expression is Identifier reference)
                instructions.Emit(OpCodes.Ldarg_0);
            else
                throw new NotImplementedException($"Cannot translate {expression.GetType().Name} to IL.");
        }

        private static ClrType ClrTypeFrom(Type type)
        {
            if (type is PrimitiveType primitive)
            {
                switch (primitive.Tag)
                {
                    case PrimitiveTypeTag.I8:
                        return typeof(sbyte);
                    case PrimitiveTypeTag.I16:
                        return typeof(short);
                    case PrimitiveTypeTag.I32:
                        return typeof(int);
                    case PrimitiveTypeTag.I64:
                        return typeof(long);
                    case PrimitiveTypeTag.U8:
                        return typeof(byte);
                    case PrimitiveTypeTag.U16:
                        return typeof(ushort);
                    case PrimitiveTypeTag.U32:
                        return typeof(uint);
                    case PrimitiveTypeTag.U64:
                        return typeof(ulong);
                    case PrimitiveTypeTag.F32:
                        return typeof(float);
                    case PrimitiveTypeTag.F64:
                        return typeof(double);
                }
            }

            Report.Error($"Cannot translate {type} to a Clr type.");

            return null;
        }

        private static ClrType ClrTypeFrom(List<Type> types)
        {
            if (types.Count == 0)
                return typeof(void);
            else if (types.Count == 1)
                return ClrTypeFrom(types[0]);
            else
                throw new NotImplementedException("Cannot translate a tuple to a Clr type.");
        }
    }
}