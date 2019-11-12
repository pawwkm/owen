using System;
using System.Diagnostics;
using System.Diagnostics.SymbolStore;
using System.IO;
using System.Reflection;
using System.Reflection.Emit;
using System.Threading;

namespace Owen
{
    internal static class IL
    {
        public static void Generate(Program program, string output)
        {
            var assembly = Thread.GetDomain().DefineDynamicAssembly(new AssemblyName(Path.GetFileNameWithoutExtension(output)), AssemblyBuilderAccess.Save);
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
                ClrTypeOf(function.Output[0].Value), 
                Type.EmptyTypes
            );

            if (method.Name == "main")
                assembly.SetEntryPoint(method.GetBaseDefinition(), PEFileKinds.ConsoleApplication);

            Generate(function.Body, method.GetILGenerator(), symbols);
        }

        private static void Generate(CompoundStatement body, ILGenerator instructions, ISymbolDocumentWriter symbols)
        {
            foreach (var statement in body.Statements)
            {
                if (statement is ReturnStatement r)
                    Generate(r, instructions, symbols);
                else
                    throw new NotImplementedException($"Cannot translate {statement.GetType().Name} to IL.");
            }
        }

        private static void Generate(ReturnStatement statement, ILGenerator instructions, ISymbolDocumentWriter symbols)
        {
            Generate(statement.Expressions[0], instructions);
            instructions.Emit(OpCodes.Ret);
        }

        private static void Generate(Expression expression, ILGenerator instructions)
        {
            if (expression is Number number)
                Generate(number, instructions);
            else
                throw new NotImplementedException($"Cannot translate {expression.GetType().Name} to IL.");
        }

        private static void Generate(Number expression, ILGenerator instructions)
        {
            switch (expression.Tag)
            {
                case NumberTag.I64:
                    instructions.Emit(OpCodes.Ldc_I8, long.Parse(expression.Value));
                    break;
                case NumberTag.I32:
                    instructions.Emit(OpCodes.Ldc_I4, int.Parse(expression.Value));
                    break;
                case NumberTag.I16:
                    instructions.Emit(OpCodes.Ldc_I4, short.Parse(expression.Value));
                    break;
                case NumberTag.I8:
                    instructions.Emit(OpCodes.Ldc_I4_S, sbyte.Parse(expression.Value));
                    break;
                case NumberTag.U64:
                    instructions.Emit(OpCodes.Ldc_I8, ulong.Parse(expression.Value));
                    break;
                case NumberTag.U32:
                    instructions.Emit(OpCodes.Ldc_I4, uint.Parse(expression.Value));
                    break;
                case NumberTag.U16:
                    instructions.Emit(OpCodes.Ldc_I4, ushort.Parse(expression.Value));
                    break;
                case NumberTag.U8:
                    instructions.Emit(OpCodes.Ldc_I4_S, byte.Parse(expression.Value));
                    break;
                case NumberTag.F32:
                    instructions.Emit(OpCodes.Ldc_R4, float.Parse(expression.Value));
                    break;
                case NumberTag.F64:
                    instructions.Emit(OpCodes.Ldc_R8, double.Parse(expression.Value));
                    break;
                default:
                    throw new NotImplementedException($"Cannot translate {expression.Tag} to IL.");
            }
        }

        private static Type ClrTypeOf(string type)
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
                    throw new NotImplementedException($"Cannot translate {type} to a Clr type.");
            }
        }
    }
}