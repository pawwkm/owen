using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Diagnostics.SymbolStore;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.Emit;

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
            foreach (var file in program.Files)
                Generate(file, assembly, module);

            module.CreateGlobalFunctions();

            var relative = Path.GetFileName(output);
            assembly.Save(relative);

            if (Path.GetFullPath(relative) != output)
            {
                if (System.IO.File.Exists(output))
                    System.IO.File.Delete(output);

                System.IO.File.Move(relative, output);
            }
        }

        private static void Generate(File file, AssemblyBuilder assembly, ModuleBuilder module)
        {
            var symbols = module.DefineDocument(file.Path, Guid.Empty, Guid.Empty, Guid.Empty);
            foreach (var function in file.Functions)
                Generate(function, assembly, module, symbols);
        }

        private static void Generate(FunctionDeclaration function, AssemblyBuilder assembly, ModuleBuilder module, ISymbolDocumentWriter symbols)
        {
            var method = module.DefineGlobalMethod
            (
                function.Name.Value,
                MethodAttributes.Assembly | MethodAttributes.Public | MethodAttributes.Static | MethodAttributes.HideBySig,
                ClrTypeOf(function.Output),
                function.Input.Select(i => ClrTypeOf(i.Type)).ToArray()
            );

            if (method.Name == "main")
                assembly.SetEntryPoint(method.GetBaseDefinition(), PEFileKinds.ConsoleApplication);

            var instructions = method.GetILGenerator();
            Generate(function.Body, instructions, symbols);
            instructions.Emit(OpCodes.Ret);
        }

        private static void Generate(CompoundStatement body, ILGenerator instructions, ISymbolDocumentWriter symbols)
        {
            foreach (var statement in body.Statements)
            {
                if (statement is ReturnStatement r)
                    Generate(r, instructions, symbols);
                else
                    Report.Error($"Cannot translate {statement.GetType().Name} to IL.");
            }
        }

        private static void Generate(ReturnStatement statement, ILGenerator instructions, ISymbolDocumentWriter symbols)
        {
            if (statement.Expressions.Count == 1)
                Generate(statement.Expressions[0], instructions);
            else if (statement.Expressions.Count > 1)
                Report.Error($"Cannot translate multiple return values to Clr.");

            instructions.Emit(OpCodes.Ret);
        }

        private static void Generate(Expression expression, ILGenerator instructions)
        {
            if (expression is Number number)
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

        private static System.Type ClrTypeOf(Type type)
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

        private static System.Type ClrTypeOf(List<Identifier> types)
        {
            if (types.Count == 0)
                return typeof(void);
            else if (types.Count == 1)
                return ClrTypeOf(types[0].Value);
            else
                throw new NotImplementedException("Cannot translate a tuple to a Clr type.");
        }

        private static System.Type ClrTypeOf(string type)
        {
            switch (type)
            {
                case "i8":
                    return typeof(sbyte);
                case "i16":
                    return typeof(short);
                case "i32":
                    return typeof(int);
                case "i64":
                    return typeof(long);
                case "u8":
                    return typeof(byte);
                case "u16":
                    return typeof(ushort);
                case "u32":
                    return typeof(uint);
                case "u64":
                    return typeof(ulong);
                default:
                    Report.Error($"Cannot translate {type} to a Clr type.");
                    break;
            }

            return null;
        }
    }
}