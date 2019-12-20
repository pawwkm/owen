using System;
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
            {
                var instructions = functionToBuilder[function].GetILGenerator();

                Generate(function.Body, instructions, functionToBuilder, symbols);
                instructions.Emit(OpCodes.Ret);
            }
        }

        private static void Generate(CompoundStatement body, ILGenerator instructions, Dictionary<FunctionDeclaration, MethodBuilder> functionToBuilder, ISymbolDocumentWriter symbols)
        {
            foreach (var statement in body.Statements)
            {
                if (statement is AssignmentStatement assignment)
                {
                    var l = 0;
                    foreach (var right in assignment.Right)
                    {
                        // https://sharplab.io/#v2:C4LglgNgPgAgTARgLACgYGYAEMEDZtyYDCmA3qppdljvjACyYCyAFAJRkVXcBuAhgCdMLPgBpMAIw4BeZgFcIwMAAcIAUwBKa4HIEA7AGp8IctQGd2Abi7dK/IQGNMsgMpg9Ac3Vad+oybUrG0wAX2DgjGw8YXdgcRwABg4mBSVVTW1dQ2NTCzZg8hRbKhgAdmEEcQAiPjMAEwAzKrZLKmCworbOykjaTFjMN09vTL8cwPzuzinuMsw4aymOkKA=

                        void Assign(Expression left, Expression r)
                        {
                            var variable = default(Symbol);
                            if (left is VariableDeclaration declaration)
                            {
                                instructions.DeclareLocal(ClrTypeFrom(declaration.Type));
                                variable = Lookup(body.Scope, declaration.Variable);
                            }
                            else if (left is Identifier reference)
                                variable = Lookup(body.Scope, reference);
                            else
                                Report.Error($"{left.Start} Cannot translate assignment to {left.GetType().Name} to IL.");

                            var isUnsigned = variable.Type is PrimitiveType p &&
                                             p.Tag >= PrimitiveTypeTag.U8 &&
                                             p.Tag <= PrimitiveTypeTag.U64;

                            if (assignment.Operator.Tag != OperatorTag.Equal)
                                Generate(left, body.Scope, instructions, functionToBuilder);

                            if (r != null)
                                Generate(r, body.Scope, instructions, functionToBuilder);

                            switch (assignment.Operator.Tag)
                            {
                                case OperatorTag.PlusEqual:
                                    instructions.Emit(OpCodes.Add);
                                    break;
                                case OperatorTag.MinusEqual:
                                    instructions.Emit(OpCodes.Sub);
                                    break;
                                case OperatorTag.MultiplyEqual:
                                    instructions.Emit(OpCodes.Mul);
                                    break;
                                case OperatorTag.DivideEqual:
                                    if (isUnsigned)
                                        instructions.Emit(OpCodes.Div_Un);
                                    else
                                        instructions.Emit(OpCodes.Div);
                                    break;
                                case OperatorTag.BitwiseAndEqual:
                                    instructions.Emit(OpCodes.And);
                                    break;
                                case OperatorTag.BitwiseOrEqual:
                                    instructions.Emit(OpCodes.Or);
                                    break;
                                case OperatorTag.BitwiseXorEqual:
                                    instructions.Emit(OpCodes.Xor);
                                    break;
                                case OperatorTag.ModuloEqual:
                                    if (isUnsigned)
                                        instructions.Emit(OpCodes.Rem_Un);
                                    else
                                        instructions.Emit(OpCodes.Rem);
                                    break;
                                case OperatorTag.LeftShiftEqual:
                                    instructions.Emit(OpCodes.Shl);
                                    break;
                                case OperatorTag.RightShiftEqual:
                                    if (isUnsigned)
                                        instructions.Emit(OpCodes.Shr_Un);
                                    else
                                        instructions.Emit(OpCodes.Shr);
                                    break;
                                case OperatorTag.Equal:
                                    // This is handled inplicitly later but we don't
                                    // want to error here.
                                    break;
                                default:
                                    Report.Error($"{assignment.Operator.Start} Cannot translate {assignment.Operator.Tag} to IL.");
                                    break;
                            }

                            if (variable.Type is PrimitiveType primitive)
                            {
                                switch (primitive.Tag)
                                {
                                    case PrimitiveTypeTag.I8:
                                        instructions.Emit(OpCodes.Conv_I1);
                                        break;
                                    case PrimitiveTypeTag.I16:
                                        instructions.Emit(OpCodes.Conv_I2);
                                        break;
                                    case PrimitiveTypeTag.I32:
                                        instructions.Emit(OpCodes.Conv_I4);
                                        break;
                                    case PrimitiveTypeTag.I64:
                                        instructions.Emit(OpCodes.Conv_I8);
                                        break;
                                    case PrimitiveTypeTag.U8:
                                        instructions.Emit(OpCodes.Conv_U1);
                                        break;
                                    case PrimitiveTypeTag.U16:
                                        instructions.Emit(OpCodes.Conv_U2);
                                        break;
                                    case PrimitiveTypeTag.U32:
                                        instructions.Emit(OpCodes.Conv_U4);
                                        break;
                                    case PrimitiveTypeTag.U64:
                                        instructions.Emit(OpCodes.Conv_U8);
                                        break;
                                    case PrimitiveTypeTag.F32:
                                        instructions.Emit(OpCodes.Conv_R4);
                                        break;
                                    case PrimitiveTypeTag.F64:
                                        instructions.Emit(OpCodes.Conv_R8);
                                        break;
                                }
                            }
                            else
                                Report.Error($"{left.Start} Cannot translate assignment to {variable.Type} to IL.");

                            if (variable is InputSymbol input)
                                instructions.Emit(OpCodes.Starg, input.Index);
                            else if (variable is LocalSymbol local)
                                instructions.Emit(OpCodes.Stloc, local.Index);
                            else
                                Report.Error($"{left.Start} Cannot translate reference to {variable.GetType().Name} to IL.");
                        }

                        if (right is Call call && call.Declaration.Output is TupleType tuple)
                        {
                            Generate(right, body.Scope, instructions, functionToBuilder);

                            var clrType = ClrTypeFrom(tuple);
                            for (var i = 0; i < tuple.Types.Count; i++)
                            {
                                instructions.Emit(OpCodes.Dup);
                                instructions.Emit(OpCodes.Ldfld, clrType.GetField($"Item{i + 1}"));

                                Assign(assignment.Left[l++], null);
                            }
                        }
                        else
                            Assign(assignment.Left[l++], right);
                    }
                }
                else if (statement is ExpressionStatement e)
                    Generate(e.Expression, body.Scope, instructions, functionToBuilder);
                else if (statement is ReturnStatement r)
                {
                    if (r.Expressions.Count == 1)
                        Generate(r.Expressions[0], body.Scope, instructions, functionToBuilder);
                    else if (r.Expressions.Count > 1)
                    {
                        foreach (var expression in r.Expressions)
                            Generate(expression, body.Scope, instructions, functionToBuilder);

                        var tuple = ClrTypeFrom(new TupleType() { Types = r.Expressions.Select(expression => expression.Type).ToList() });
                        instructions.Emit(OpCodes.Newobj, tuple.GetConstructors().First());

                        // push every expression to the stack
                        // new instance of return ValueType.
                        // Report.Error($"Cannot translate multiple return values to Clr."); // https://sharplab.io/#v2:C4LglgNgPgAgTARgLACgYGYAEMEDZtyYDCmA3qppdljvjACyYCyAFAJRkVXcBuAhgCdMLPgBpMAIw4BeZgFcIwMAAcIAUwBKa4HIEA7AGp8IctQGd2Abi7dK/IQGNMsgMpg9Ac3Vad+oybUrG0wAX2DgjGw8YXdgcRwABg4mBSVVTW1dQ2NTCzZg8hRbKhgAdmEEcQAiPjMAEwAzKrZLKmCworbOykjaTFjMN09vTL8cwPzuzinuMsw4aymOkKA=
                    }

                    instructions.Emit(OpCodes.Ret);
                }
                else if (statement is AssertStatement assert)
                {
                    Generate(assert.Assertion, body.Scope, instructions, functionToBuilder);
                    instructions.Emit(OpCodes.Call, typeof(Debug).GetMethod("Assert", new[] { typeof(bool) }));
                }
                else
                    Report.Error($"Cannot translate {statement.GetType().Name} to IL.");
            }
        }

        private static void Generate(Expression expression, Scope scope, ILGenerator instructions, Dictionary<FunctionDeclaration, MethodBuilder> functionToBuilder)
        {
            if (expression is BinaryExpression binary)
            {
                Generate(binary.Left, scope, instructions, functionToBuilder);
                Generate(binary.Right, scope, instructions, functionToBuilder);

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
                        Report.Error($"{binary.Operator.Start} Cannot translate {binary.Operator.Tag} to IL.");
                        break;
                }
            }
            else if (expression is Call call)
            {
                if (call.Arguments.Count != 0)
                    Report.Error($"{call.Reference.Start} Cannot translate arguments to IL.");

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
                        instructions.Emit(OpCodes.Ldc_I4, int.Parse(number.Value));
                        break;
                    case NumberTag.I8:
                        instructions.Emit(OpCodes.Ldc_I4_S, sbyte.Parse(number.Value));
                        break;
                    case NumberTag.U64:
                        instructions.Emit(OpCodes.Ldc_I4, int.Parse(number.Value));
                        instructions.Emit(OpCodes.Conv_I8);
                        break;
                    case NumberTag.U32:
                        instructions.Emit(OpCodes.Ldc_I4, uint.Parse(number.Value));
                        break;
                    case NumberTag.U16:
                        instructions.Emit(OpCodes.Ldc_I4, uint.Parse(number.Value));
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
                        Report.Error($"{number.Start} Cannot translate {number.Tag} to IL.");
                        break;
                }
            }
            else if (expression is Identifier reference)
            {
                var symbol = Lookup(scope, reference);
                if (symbol is InputSymbol input)
                    instructions.Emit(OpCodes.Ldarg, input.Index);
                else if (symbol is LocalSymbol local)
                    instructions.Emit(OpCodes.Ldloc, local.Index);
                else
                    Report.Error($"{reference.Start} Cannot translate reference to {symbol.GetType().Name} to IL.");
            }
            else
                Report.Error($"{expression.Start} Cannot translate {expression.GetType().Name} to IL.");
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
            else if (type is TupleType tuple)
            {
                switch (tuple.Types.Count)
                {
                    case 2:
                        return typeof(ValueTuple<,>).MakeGenericType
                        (
                            ClrTypeFrom(tuple.Types[0]), 
                            ClrTypeFrom(tuple.Types[1])
                        );
                    case 3:
                        return typeof(ValueTuple<,,>).MakeGenericType
                        (
                            ClrTypeFrom(tuple.Types[0]),
                            ClrTypeFrom(tuple.Types[1]),
                            ClrTypeFrom(tuple.Types[2])
                        );
                    case 4:
                        return typeof(ValueTuple<,,,>).MakeGenericType
                        (
                            ClrTypeFrom(tuple.Types[0]),
                            ClrTypeFrom(tuple.Types[1]),
                            ClrTypeFrom(tuple.Types[2]),
                            ClrTypeFrom(tuple.Types[3])
                        );
                    case 5:
                        return typeof(ValueTuple<,,,,>).MakeGenericType
                        (
                            ClrTypeFrom(tuple.Types[0]),
                            ClrTypeFrom(tuple.Types[1]),
                            ClrTypeFrom(tuple.Types[2]),
                            ClrTypeFrom(tuple.Types[3]),
                            ClrTypeFrom(tuple.Types[4])
                        );
                    case 6:
                        return typeof(ValueTuple<,,,,,>).MakeGenericType
                        (
                            ClrTypeFrom(tuple.Types[0]),
                            ClrTypeFrom(tuple.Types[1]),
                            ClrTypeFrom(tuple.Types[2]),
                            ClrTypeFrom(tuple.Types[3]),
                            ClrTypeFrom(tuple.Types[4]),
                            ClrTypeFrom(tuple.Types[5])
                        );
                    case 7:
                        return typeof(ValueTuple<,,,,,,>).MakeGenericType
                        (
                            ClrTypeFrom(tuple.Types[0]),
                            ClrTypeFrom(tuple.Types[1]),
                            ClrTypeFrom(tuple.Types[2]),
                            ClrTypeFrom(tuple.Types[3]),
                            ClrTypeFrom(tuple.Types[4]),
                            ClrTypeFrom(tuple.Types[5]),
                            ClrTypeFrom(tuple.Types[6])
                        );
                    default:
                        Report.Error($"Cannot translate a tuple of {tuple.Types.Count} values to a Clr type.");
                        break;
                }
            }
            else if (type == null)
                return typeof(void);
            
            Report.Error($"Cannot translate {type} to a Clr type.");

            return null;
        }

        private static Symbol Lookup(Scope scope, Identifier name)
        {
            foreach (var symbol in scope.Symbols)
            {
                if (symbol.Name == name.Value)
                    return symbol;
            }

            if (scope.Parent == null)
                return null;
            else
                return Lookup(scope.Parent, name);
        }
    }
}