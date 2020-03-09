// https://docs.microsoft.com/en-us/cpp/c-runtime-library/crt-library-features?view=vs-2019
// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/malloc?view=vs-2019

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.Emit;
using System.Runtime.InteropServices;
using ClrType = System.Type;

internal static class IL
{
    internal static Action Generate(Program program, bool includePropositions, string executable, PEFileKinds ui)
    {
        var name = Path.GetFileName(executable);
        var assembly = AppDomain.CurrentDomain.DefineDynamicAssembly(new AssemblyName(name), AssemblyBuilderAccess.RunAndSave, Path.GetDirectoryName(executable));
        var module = assembly.DefineDynamicModule("MainModule", name, true);

        var functionToBuilder = new Dictionary<FunctionDeclaration, MethodBuilder>();
        var methodAttributes = MethodAttributes.Public | MethodAttributes.Static | MethodAttributes.HideBySig;

        foreach (var file in program.Files)
        {
            foreach (var function in file.Functions)
            {
                if (function.IsExternal && function.Generalized.Count != 0 || function.Generalized.Count == 0)
                    Define(function);
                else
                {
                    foreach (var resolvedFunction in function.Resolved)
                        Define(resolvedFunction);
                }

                void Define(FunctionDeclaration f)
                {
                    var builder = default(MethodBuilder);
                    var input = f.Input.Select(i => ClrTypeFrom(i.Type, module)).ToArray();
                    var output = ClrTypeFrom(f.Output, module);

                    if (f.IsExternal)
                    {
                        builder = module.DefinePInvokeMethod(f.Name.Value, f.Library.Value, methodAttributes, CallingConventions.Standard, output, input, CallingConvention.Cdecl, CharSet.Unicode);

                        // If this is missing then it will blow up if executed in memory.
                        builder.SetImplementationFlags(builder.GetMethodImplementationFlags() | MethodImplAttributes.PreserveSig);
                    }
                    else
                        builder = module.DefineGlobalMethod(f.Name.Value, methodAttributes, output, input);

                    functionToBuilder.Add(f, builder);
                }
            }
        }

        // do some mem copy trick on true main for the string[] from the OS.
        // call System.Text.Encoding.UTF8.GetBytes(string) on every string the memcopy it to [][]U8 and pass it to main.
        var trueMainFunction = module.DefineGlobalMethod($"_true_main", methodAttributes, typeof(int), new[] { typeof(string[]) });
        assembly.SetEntryPoint(trueMainFunction.GetBaseDefinition(), ui);

        var mainBody = trueMainFunction.GetILGenerator();
        if (includePropositions)
        {
            var counter = 0;
            foreach (var file in program.Files)
            {
                foreach (var proposition in file.Propositions)
                {
                    var builder = module.DefineGlobalMethod($"_proposition{counter++}", methodAttributes, typeof(void), ClrType.EmptyTypes);
                    var instructions = builder.GetILGenerator();
                    var exit = instructions.DefineLabel();

                    Generate(proposition, instructions, functionToBuilder, true, exit, module, default);
                    instructions.MarkLabel(exit);
                    instructions.Emit(OpCodes.Ret);

                    mainBody.EmitCall(OpCodes.Call, builder, ClrType.EmptyTypes);
                }
            }
        }

        mainBody.EmitCall(OpCodes.Call, functionToBuilder.First(p => p.Key.Name.Value == "main").Value, ClrType.EmptyTypes);
        mainBody.Emit(OpCodes.Ret);

        foreach (var (function, builder) in functionToBuilder.Where(p => !p.Key.IsExternal))
        {
            var instructions = builder.GetILGenerator();
            var exit = instructions.DefineLabel();
            Generate(function.Body, instructions, functionToBuilder, false, exit, module, default);
            instructions.MarkLabel(exit);
            instructions.Emit(OpCodes.Ret);
        }

        module.CreateGlobalFunctions();
        assembly.Save(name);

        return () => assembly.GetModules()[1].GetMethod("_true_main").Invoke(null, new object[] { new string[0] });
    }

    private static void Generate(AssignmentStatement assignmentStatement, Scope scope, ILGenerator instructions, Dictionary<FunctionDeclaration, MethodBuilder> functionToBuilder, ModuleBuilder module)
    {
        if (assignmentStatement.Left.Count > 1 && assignmentStatement.Right[0] is Call call && call.Declaration.Output is TupleType tupleType)
        {
            if (assignmentStatement.Operator.Tag != OperatorTag.Equal)
                Report.InternalError($"Cannot translate the {assignmentStatement.Operator.Tag} operator in tuple assignment to IL.");

            Generate(call, scope, instructions, functionToBuilder, module);
            for (var i = 1; i < tupleType.Types.Count; i++)
                instructions.Emit(OpCodes.Dup);

            var clrTuple = ClrTypeFrom(tupleType, module);
            for (var i = 0; i < assignmentStatement.Left.Count; i++)
            {
                var variable = default(Symbol);
                if (assignmentStatement.Left[i] is VariableDeclaration declaration)
                {
                    var local = (LocalSymbol)Lookup(declaration.Variable, scope);
                    local.Index = (ushort)instructions.DeclareLocal(ClrTypeFrom(declaration.Type, module)).LocalIndex;

                    variable = local;
                }
                else if (assignmentStatement.Left[i] is Identifier identifier)
                    variable = Lookup(identifier, scope);
                else
                    Report.InternalError($"{assignmentStatement.Left[i].Start} Cannot translate assignment to {assignmentStatement.Left[i].GetType().Name} to IL.");

                instructions.Emit(OpCodes.Ldfld, clrTuple.GetField($"Item{i + 1}"));

                if (variable is InputSymbol input)
                    instructions.Emit(OpCodes.Starg, input.Index);
                else if (variable is LocalSymbol local)
                    instructions.Emit(OpCodes.Stloc, local.Index);
                else
                    Report.InternalError($"{assignmentStatement.Left[i].Start} Cannot translate reference to {variable.GetType().Name} to IL.");
            }
        }
        else
        {
            for (var i = 0; i < assignmentStatement.Left.Count; i++)
            {
                var typeBeingPointedTo = default(Type);
                var isDereferencedStore = false;
                var storedInField = default(FieldInfo);
                var symbol = default(Symbol);
                if (assignmentStatement.Left[i] is Identifier identifier)
                    symbol = Lookup(identifier, scope);
                else if (assignmentStatement.Left[i] is VariableDeclaration variableDeclaration)
                {
                    var local = (LocalSymbol)Lookup(variableDeclaration.Variable, scope);
                    local.Index = (ushort)instructions.DeclareLocal(ClrTypeFrom(variableDeclaration.Type, module)).LocalIndex;

                    symbol = local;
                }
                else if (assignmentStatement.Left[i] is FieldAccess fieldAccess)
                {
                    symbol = Lookup((Identifier)fieldAccess.Object, scope);

                    LoadAddress(symbol, instructions);
                    while (fieldAccess.Field is FieldAccess)
                    {
                        var type = fieldAccess.Object.Type;
                        fieldAccess = (FieldAccess)fieldAccess.Field;

                        instructions.Emit(OpCodes.Ldflda, ClrTypeFrom(type, module).GetField(((Identifier)fieldAccess.Object).Value));
                    }

                    storedInField = ClrTypeFrom(fieldAccess.Object.Type, module).GetField(((Identifier)fieldAccess.Field).Value);
                }
                else if (assignmentStatement.Left[i] is Dereference dereference)
                {
                    isDereferencedStore = true;
                    if (dereference.Expression is Dereference)
                        Report.InternalError("Cannot not dereference multiple steps yet.");
                    else if (dereference.Expression is FieldAccess)
                    {
                        fieldAccess = dereference.Expression as FieldAccess;
                        symbol = Lookup((Identifier)fieldAccess.Object, scope);

                        LoadAddress(symbol, instructions);
                        while (fieldAccess.Field is FieldAccess)
                        {
                            var type = fieldAccess.Object.Type;
                            fieldAccess = (FieldAccess)fieldAccess.Field;

                            instructions.Emit(OpCodes.Ldfld, ClrTypeFrom(type, module).GetField(((Identifier)fieldAccess.Object).Value));
                        }

                        Generate(fieldAccess.Field, scope, instructions, functionToBuilder, module, fieldAccess.Object.Type);
                        typeBeingPointedTo = ((PointerType)fieldAccess.Field.Type).To;
                    }
                    else
                    {
                        symbol = Lookup((Identifier)dereference.Expression, scope);
                        typeBeingPointedTo = ((PointerType)symbol.Type).To;
                        LoadValue(symbol, instructions);
                    }
                }
                else
                    Report.InternalError($"{assignmentStatement.Left[i].Start} Cannot figure out how to store {assignmentStatement.Left[i].GetType().Name} in IL.");

                var isUnsigned = assignmentStatement.Left[i].Type is PrimitiveType p &&
                                 p.Tag >= PrimitiveTypeTag.U8 &&
                                 p.Tag <= PrimitiveTypeTag.U64;

                if (assignmentStatement.Operator.Tag != OperatorTag.Equal)
                    Generate(assignmentStatement.Left[i], scope, instructions, functionToBuilder, module);

                Generate(assignmentStatement.Right[i], scope, instructions, functionToBuilder, module);
                // https://sharplab.io/#v2:C4LglgNgNAJiDUAfAAgJgIwFgBQyDMABGgQMIEDeOB1RhyALAQLIAUAlBVTdwG4CGAJwJ8CAXgIAGANxdu1EfHGoZ2OTREBacehVr5BAFTiArLr0iA9ErNqRAUnF4bckQDJHz7iMQfZLggB64vSe6gQAPOHWfl4EAHxx0armYgT0yn4AvjiZQA==
                switch (assignmentStatement.Operator.Tag)
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
                        Report.InternalError($"{assignmentStatement.Operator.Start} Cannot translate {assignmentStatement.Operator.Tag} to IL.");
                        break;
                }

                // convert back here? Ex. conv.i1 for I8. 
                if (assignmentStatement.Left[i].Type is PrimitiveType primitive)
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
                //else
                //    Report.InternalError($"{assignmentStatement.Left[i].Start} Cannot translate assignment to {variable.Type} to IL.");

                if (isDereferencedStore)
                {
                    
                    if (typeBeingPointedTo == Semantics.I8 || typeBeingPointedTo == Semantics.U8)
                        instructions.Emit(OpCodes.Stind_I1);
                    else if (typeBeingPointedTo == Semantics.I16 || typeBeingPointedTo == Semantics.U16)
                        instructions.Emit(OpCodes.Stind_I2);
                    else if (typeBeingPointedTo == Semantics.I32 || typeBeingPointedTo == Semantics.I32)
                        instructions.Emit(OpCodes.Stind_I4);
                    else if (typeBeingPointedTo == Semantics.I64 || typeBeingPointedTo == Semantics.U64)
                        instructions.Emit(OpCodes.Stind_I8);
                    else if (typeBeingPointedTo == Semantics.F32)
                        instructions.Emit(OpCodes.Stind_R4);
                    else if (typeBeingPointedTo == Semantics.F64)
                        instructions.Emit(OpCodes.Stind_R8);
                    else
                        Report.InternalError("How did you end up here?");
                }
                else if (storedInField != null)
                    instructions.Emit(OpCodes.Stfld, storedInField);
                else if (symbol is LocalSymbol localSymbol)
                {
                    if (localSymbol.Index == 0)
                        instructions.Emit(OpCodes.Stloc_0);
                    else if (localSymbol.Index == 1)
                        instructions.Emit(OpCodes.Stloc_1);
                    else if (localSymbol.Index == 2)
                        instructions.Emit(OpCodes.Stloc_2);
                    else if (localSymbol.Index == 3)
                        instructions.Emit(OpCodes.Stloc_3);
                    else if (localSymbol.Index > byte.MaxValue)
                        instructions.Emit(OpCodes.Stloc, localSymbol.Index);
                    else
                        instructions.Emit(OpCodes.Stloc_S, (byte)localSymbol.Index);
                }
                else if (symbol is InputSymbol inputSymbol)
                {
                    if (inputSymbol.Index > byte.MaxValue)
                        instructions.Emit(OpCodes.Starg, inputSymbol.Index);
                    else
                        instructions.Emit(OpCodes.Starg_S, (byte)inputSymbol.Index);
                }
                else
                    Report.InternalError("How did you get here?");
            }
        }
    }

    private static void Generate(CompoundStatement body, ILGenerator instructions, Dictionary<FunctionDeclaration, MethodBuilder> functionToBuilder, bool isProposition, Label exit, ModuleBuilder module, Label endOfLoop)
    {
        instructions.BeginScope();
        foreach (var statement in body.Statements)
        {
            if (statement is AssignmentStatement assignmentStatement)
                Generate(assignmentStatement, body.Scope, instructions, functionToBuilder, module);
            else if (statement is IfStatement ifStatement)
            {
                var endOfStatement = instructions.DefineLabel();
                foreach (var branch in ifStatement.Branches)
                {
                    if (branch.Assignment != null)
                    {
                        instructions.BeginScope();
                        Generate(branch.Assignment, branch.Body.Scope, instructions, functionToBuilder, module);
                    }

                    if (branch.Condition != null)
                        Generate(branch.Condition, branch.Body.Scope, instructions, functionToBuilder, module);

                    var next = instructions.DefineLabel();
                    if (branch.Condition != null)
                        instructions.Emit(OpCodes.Brfalse, next);

                    Generate(branch.Body, instructions, functionToBuilder, isProposition, exit, module, default);

                    instructions.Emit(OpCodes.Br, endOfStatement);
                    if (branch.Condition != null)
                        instructions.MarkLabel(next);
                }

                instructions.MarkLabel(endOfStatement);
                foreach (var branch in ifStatement.Branches)
                {
                    if (branch.Assignment != null)
                        instructions.EndScope();
                }
            }
            else if (statement is ForStatement forStatement)
            {
                // https://sharplab.io/#v2:C4LglgNgNAJiDUAfAAgJgIwFgBQPkGYACNQgYUJwG8dDbijkAWQgWQAoBKC7Ow6n3nQBmAewBOhNgDcAhhLCEAvIXQBuMiIB2AZxEQApgDoASvpkwAMmE37OS5QCIAxlphhgYLQ/Vh48DjSCtPxBQcjoAJxsYByqgUEAvvGESdgJQA==
                instructions.BeginScope();
                Generate(forStatement.Assignment, forStatement.Body.Scope, instructions, functionToBuilder, module);

                var forHead = instructions.DefineLabel();
                var forBody = instructions.DefineLabel();
                var forEnd = instructions.DefineLabel();

                instructions.Emit(OpCodes.Br, forHead);
                instructions.MarkLabel(forBody);

                Generate(forStatement.Body, instructions, functionToBuilder, isProposition, exit, module, forEnd);

                instructions.MarkLabel(forHead);
                Generate(forStatement.Post, forStatement.Body.Scope, instructions, functionToBuilder, module);

                Generate(forStatement.Condition, forStatement.Body.Scope, instructions, functionToBuilder, module);
                instructions.Emit(OpCodes.Brtrue, forBody);

                instructions.MarkLabel(forEnd);
                instructions.EndScope();
            }
            else if (statement is WhileStatement whileStatement)
            {
                // https://sharplab.io/#v2:C4LglgNgNAJiDUAfAAgJgIwFgBQPkGYACNQgYUJwG8dDbijkAWQgWQAoBKC7Ow6n3nQDuAC0gBTQm1IB7AHYBnGRHEA6AEriAhjAAyYOeM6EAvCcIAiAMbyYYYGHkWONQbX5u3ydAE42FgCMZGABPZwBuVzcAXyjCWOxooA=
                if (whileStatement.Assignment != null)
                {
                    instructions.BeginScope();
                    Generate(whileStatement.Assignment, whileStatement.Body.Scope, instructions, functionToBuilder, module);
                }

                var whileHead = instructions.DefineLabel();
                var whileBody = instructions.DefineLabel();
                var whileEnd = instructions.DefineLabel();

                instructions.Emit(OpCodes.Br, whileHead);
                instructions.MarkLabel(whileBody);

                Generate(whileStatement.Body, instructions, functionToBuilder, isProposition, exit, module, whileEnd);

                instructions.MarkLabel(whileHead);
                Generate(whileStatement.Condition, whileStatement.Body.Scope, instructions, functionToBuilder, module);
                instructions.Emit(OpCodes.Brtrue, whileBody);

                instructions.MarkLabel(whileEnd);
                if (whileStatement.Assignment != null)
                    instructions.EndScope();
            }
            else if (statement is ExpressionStatement expressionStatement)
            {
                Generate(expressionStatement.Expression, body.Scope, instructions, functionToBuilder, module);
                if (expressionStatement.Expression is Call call && call.Declaration.Output != null)
                    instructions.Emit(OpCodes.Pop);
            }
            else if (statement is BreakStatement)
                instructions.Emit(OpCodes.Br, endOfLoop);
            else if (statement is ReturnStatement returnStatement)
            {
                foreach (var output in returnStatement.Expressions)
                    Generate(output, body.Scope, instructions, functionToBuilder, module);

                if (returnStatement.Expressions.Count > 1)
                    instructions.Emit(OpCodes.Newobj, ClrTypeFrom(new TupleType() { Types = returnStatement.Expressions.Select(e => e.Type).ToList() }, module).GetConstructors()[0]); // There is only one constructor.

                instructions.Emit(OpCodes.Br, exit);
            }
            else if (statement is AssertStatement assertStatement)
            {
                var happyPath = instructions.DefineLabel();

                Generate(assertStatement.Assertion, body.Scope, instructions, functionToBuilder, module);
                instructions.Emit(OpCodes.Brtrue, happyPath);

                instructions.EmitCall(OpCodes.Call, typeof(Console).GetProperty("Error").GetGetMethod(), null);
                instructions.Emit(OpCodes.Ldstr, $"{assertStatement.Assertion.Start} Assert tripped.");
                instructions.EmitCall(OpCodes.Callvirt, typeof(TextWriter).GetMethod("WriteLine", new[] { typeof(string) }), null);

                if (isProposition)
                    instructions.Emit(OpCodes.Ret);
                else
                {
                    instructions.Emit(OpCodes.Ldc_I4_1);
                    instructions.EmitCall(OpCodes.Call, typeof(Environment).GetMethod("Exit", new[] { typeof(int) }), null);
                }

                instructions.MarkLabel(happyPath);
            }
            else
                Report.InternalError($"Cannot translate {statement.GetType()} to IL.");
        }

        instructions.EndScope();
    }

    private static void Generate(Expression expression, Scope scope, ILGenerator instructions, Dictionary<FunctionDeclaration, MethodBuilder> functionToBuilder, ModuleBuilder module, Type previous = null)
    {
        if (expression is BinaryExpression binaryExpression)
        {
            if (binaryExpression.Operator.Tag == OperatorTag.LogicalOr)
            {
                var rhs = instructions.DefineLabel();
                var end = instructions.DefineLabel();

                Generate(binaryExpression.Left, scope, instructions, functionToBuilder, module);
                instructions.Emit(OpCodes.Brfalse, rhs);
                instructions.Emit(OpCodes.Ldc_I4_1);
                instructions.Emit(OpCodes.Br, end);

                instructions.MarkLabel(rhs);
                Generate(binaryExpression.Right, scope, instructions, functionToBuilder, module);

                instructions.MarkLabel(end);
            }
            else if (binaryExpression.Operator.Tag == OperatorTag.LogicalAnd)
            {
                var rhs = instructions.DefineLabel();
                var end = instructions.DefineLabel();

                Generate(binaryExpression.Left, scope, instructions, functionToBuilder, module);
                instructions.Emit(OpCodes.Brtrue, rhs);
                instructions.Emit(OpCodes.Ldc_I4_0);
                instructions.Emit(OpCodes.Br, end);

                instructions.MarkLabel(rhs);
                Generate(binaryExpression.Right, scope, instructions, functionToBuilder, module);

                instructions.MarkLabel(end);
            }
            else
            {
                Generate(binaryExpression.Left, scope, instructions, functionToBuilder, module);
                Generate(binaryExpression.Right, scope, instructions, functionToBuilder, module);

                var isUnsigned = false;
                if (binaryExpression.Left.Type is PrimitiveType lhst && binaryExpression.Right.Type is PrimitiveType)
                    isUnsigned = lhst.Tag >= PrimitiveTypeTag.U8 && lhst.Tag <= PrimitiveTypeTag.U64;

                switch (binaryExpression.Operator.Tag)
                {
                    case OperatorTag.EqualEqual:
                        instructions.Emit(OpCodes.Ceq);
                        break;
                    case OperatorTag.NotEqual:
                        instructions.Emit(OpCodes.Ceq);
                        instructions.Emit(OpCodes.Ldc_I4_0);
                        instructions.Emit(OpCodes.Ceq);
                        break;
                    case OperatorTag.LessThanOrEqual:
                        if (isUnsigned)
                            instructions.Emit(OpCodes.Cgt_Un);
                        else
                            instructions.Emit(OpCodes.Cgt);

                        instructions.Emit(OpCodes.Ldc_I4_0);
                        instructions.Emit(OpCodes.Ceq);
                        break;
                    case OperatorTag.GreaterThanOrEqual:
                        if (isUnsigned)
                            instructions.Emit(OpCodes.Clt_Un);
                        else
                            instructions.Emit(OpCodes.Clt);

                        instructions.Emit(OpCodes.Ldc_I4_0);
                        instructions.Emit(OpCodes.Ceq);
                        break;
                    case OperatorTag.LessThan:
                        if (isUnsigned)
                            instructions.Emit(OpCodes.Clt_Un);
                        else
                            instructions.Emit(OpCodes.Clt);
                        break;
                    case OperatorTag.GreaterThan:
                        if (isUnsigned)
                            instructions.Emit(OpCodes.Cgt_Un);
                        else
                            instructions.Emit(OpCodes.Cgt);
                        break;
                    case OperatorTag.Plus:
                        instructions.Emit(OpCodes.Add);
                        break;
                    case OperatorTag.Minus:
                        instructions.Emit(OpCodes.Sub);
                        break;
                    case OperatorTag.BitwiseOr:
                        instructions.Emit(OpCodes.Or);
                        break;
                    case OperatorTag.BitwiseXor:
                        instructions.Emit(OpCodes.Xor);
                        break;
                    case OperatorTag.Multiply:
                        instructions.Emit(OpCodes.Mul);
                        break;
                    case OperatorTag.Divide:
                        if (isUnsigned)
                            instructions.Emit(OpCodes.Div_Un);
                        else
                            instructions.Emit(OpCodes.Div);
                        break;
                    case OperatorTag.Modulo:
                        if (isUnsigned)
                            instructions.Emit(OpCodes.Rem_Un);
                        else
                            instructions.Emit(OpCodes.Rem);
                        break;
                    case OperatorTag.BitwiseAnd:
                        instructions.Emit(OpCodes.And);
                        break;
                    case OperatorTag.LeftShift:
                        instructions.Emit(OpCodes.Shl);
                        break;
                    case OperatorTag.RightShift:
                        instructions.Emit(OpCodes.Shr);
                        break;
                    default:
                        Report.InternalError($"{binaryExpression.Operator.Start} Cannot translate {binaryExpression.Operator.Tag} to IL.");
                        break;
                }
            }
        }
        else if (expression is Call call)
        {
            foreach (var argument in call.Arguments)
                Generate(argument, scope, instructions, functionToBuilder, module);

            instructions.EmitCall(OpCodes.Call, functionToBuilder[call.Declaration], null);
        }
        else if (expression is Negate negate)
        {
            Generate(negate.Expression, scope, instructions, functionToBuilder, module);
            instructions.Emit(OpCodes.Neg);
        }
        else if (expression is Not not)
        {
            Generate(not.Expression, scope, instructions, functionToBuilder, module);
            instructions.Emit(OpCodes.Not);
        }
        else if (expression is Number number)
        {
            if (number.Tag <= NumberTag.I64)
            {
                var value = Convert.ToInt64(number.Value);
                if (value == -1)
                    instructions.Emit(OpCodes.Ldc_I4_M1);
                else if (value == 0)
                    instructions.Emit(OpCodes.Ldc_I4_0);
                else if (value == 1)
                    instructions.Emit(OpCodes.Ldc_I4_1);
                else if (value == 2)
                    instructions.Emit(OpCodes.Ldc_I4_2);
                else if (value == 3)
                    instructions.Emit(OpCodes.Ldc_I4_3);
                else if (value == 4)
                    instructions.Emit(OpCodes.Ldc_I4_4);
                else if (value == 5)
                    instructions.Emit(OpCodes.Ldc_I4_5);
                else if (value == 6)
                    instructions.Emit(OpCodes.Ldc_I4_6);
                else if (value == 7)
                    instructions.Emit(OpCodes.Ldc_I4_7);
                else if (value == 8)
                    instructions.Emit(OpCodes.Ldc_I4_8);
                else if (value <= sbyte.MaxValue || value >= sbyte.MinValue)
                    instructions.Emit(OpCodes.Ldc_I4_S, (sbyte)value);
                else if (value <= int.MaxValue || value >= int.MinValue)
                    instructions.Emit(OpCodes.Ldc_I4_S, (int)value);
                else
                    instructions.Emit(OpCodes.Ldc_I8, value);

                if (number.Tag == NumberTag.I64 && value <= int.MaxValue)
                    instructions.Emit(OpCodes.Conv_U8);
                else if (number.Tag == NumberTag.I16)
                    instructions.Emit(OpCodes.Conv_I2);
                else if (number.Tag == NumberTag.I8)
                    instructions.Emit(OpCodes.Conv_I1);
            }
            else if (number.Tag <= NumberTag.U64)
            {
                var value = Convert.ToUInt64(number.Value);
                if (value == 0)
                    instructions.Emit(OpCodes.Ldc_I4_0);
                else if (value == 1)
                    instructions.Emit(OpCodes.Ldc_I4_1);
                else if (value == 2)
                    instructions.Emit(OpCodes.Ldc_I4_2);
                else if (value == 3)
                    instructions.Emit(OpCodes.Ldc_I4_3);
                else if (value == 4)
                    instructions.Emit(OpCodes.Ldc_I4_4);
                else if (value == 5)
                    instructions.Emit(OpCodes.Ldc_I4_5);
                else if (value == 6)
                    instructions.Emit(OpCodes.Ldc_I4_6);
                else if (value == 7)
                    instructions.Emit(OpCodes.Ldc_I4_7);
                else if (value == 8)
                    instructions.Emit(OpCodes.Ldc_I4_8);
                else if (value <= byte.MaxValue)
                    instructions.Emit(OpCodes.Ldc_I4_S, (byte)value);
                else if (value == uint.MaxValue)
                    instructions.Emit(OpCodes.Ldc_I4_M1);
                else if (value < uint.MaxValue)
                    instructions.Emit(OpCodes.Ldc_I4, (uint)value);
                else
                    instructions.Emit(OpCodes.Ldc_I8, value);

                if (number.Tag == NumberTag.U64 && value <= uint.MaxValue)
                    instructions.Emit(OpCodes.Conv_U8);
                else if (number.Tag == NumberTag.U16)
                    instructions.Emit(OpCodes.Conv_U2);
                else if (number.Tag == NumberTag.U8)
                    instructions.Emit(OpCodes.Conv_U1);
            }
            else if (number.Tag == NumberTag.F32)
                instructions.Emit(OpCodes.Ldc_R4, (float)number.Value);
            else if (number.Tag == NumberTag.F64)
                instructions.Emit(OpCodes.Ldc_R8, (double)number.Value);
            else
                Report.InternalError($"Cannot translate {number.Tag} to IL.");
        }
        else if (expression is Boolean boolean)
            instructions.Emit(boolean.Value == "true" ? OpCodes.Ldc_I4_1 : OpCodes.Ldc_I4_0);
        else if (expression is Identifier identifier)
        {
            if (previous == null)
            {
                var symbol = Lookup(identifier, scope);
                if (symbol is LocalSymbol || symbol is InputSymbol)
                    LoadValue(symbol, instructions);
                //else this is an enumeration or compound
            }
            else if (previous is EnumerationDeclaration enumerationDeclaration)
                Generate(enumerationDeclaration.Constants.First(c => c.Name.Value == identifier.Value).Value, scope, instructions, functionToBuilder, module);
            else if (previous is CompoundDeclaration compoundDeclaration)
            {
                var field = compoundDeclaration.Fields.First(f => f.Name.Value == identifier.Value);
                instructions.Emit(OpCodes.Ldfld, ClrTypeFrom(previous, module).GetField(field.Name.Value));
            }
            else if (previous is ArrayType)
                instructions.Emit(OpCodes.Ldfld, ClrTypeFrom(previous, module).GetField(identifier.Value));
            else
                Report.InternalError("How did you get here?");
        }
        else if (expression is SizeOf sizeOf)
            GenerateSizeOf(sizeOf.TypeBeingSizedUp, instructions, module);
        else if (expression is CompoundLiteral compoundLiteral)
        {
            instructions.BeginScope();
            var clrType = ClrTypeFrom(compoundLiteral.Type, module);
            var temp = instructions.DeclareLocal(clrType, true);

            foreach (var initializer in compoundLiteral.Initializers)
            {
                LoadLocalAddress((ushort)temp.LocalIndex, instructions);

                Generate(initializer.Value, scope, instructions, functionToBuilder, module);
                instructions.Emit(OpCodes.Stfld, clrType.GetField(initializer.Name.Value));
            }

            LoadLocal((ushort)temp.LocalIndex, instructions);

            instructions.EndScope();
        }
        else if (expression is FieldAccess fieldAccess)
        {
            Generate(fieldAccess.Object, scope, instructions, functionToBuilder, module, previous);
            if (fieldAccess.Field is Identifier || fieldAccess.Field is FieldAccess)
                Generate(fieldAccess.Field, scope, instructions, functionToBuilder, module, fieldAccess.Object.Type);
            else
                Report.InternalError($"Cannot translate {fieldAccess.Field.GetType().Name} to IL.");
        }
        else if (expression is Dereference dereference)
        {
            Generate(dereference.Expression, scope, instructions, functionToBuilder, module, previous);

            var type = dereference.Type; // this should be the type being pointed to.
            if (type == Semantics.I8 || type == Semantics.U8)
                instructions.Emit(OpCodes.Ldind_I1);
            else if (type == Semantics.I16)
                instructions.Emit(OpCodes.Ldind_I2);
            else if (type == Semantics.I32)
                instructions.Emit(OpCodes.Ldind_I4);
            else if (type == Semantics.I64)
                instructions.Emit(OpCodes.Ldind_I8);
            else if (type == Semantics.U8)
                instructions.Emit(OpCodes.Ldind_U1);
            else if (type == Semantics.U16)
                instructions.Emit(OpCodes.Ldind_U2);
            else if (type == Semantics.U32)
                instructions.Emit(OpCodes.Ldind_U4);
            else if (type == Semantics.U64)
                instructions.Emit(OpCodes.Ldind_I8);
            else if (type is PointerType)
                instructions.Emit(OpCodes.Ldind_I); 
            else 
                Report.InternalError($"Cannot dereference {dereference.Expression.GetType()}.");
        }
        else if (expression is NullLiteral)
            instructions.Emit(OpCodes.Ldnull);
        else if (expression is AddressOf addressOf)
        {
            if (addressOf.Expression is Identifier)
                LoadAddress(Lookup((Identifier)addressOf.Expression, scope), instructions);
            else
                Report.InternalError($"Cannot translate {nameof(AddressOf)} {addressOf.Expression.GetType()}  to IL.");
        }
        else if (expression is ArrayLiteral arrayLiteral)
        {
            instructions.BeginScope();
            var clrType = ClrTypeFrom(arrayLiteral.Type, module);
            var temp = instructions.DeclareLocal(clrType, true);

            LoadLocal((ushort)temp.LocalIndex, instructions);
            instructions.EndScope();
        }
        else if (expression is ArrayAccess arrayAccess)
        {
            Generate(arrayAccess.Array, scope, instructions, functionToBuilder, module, previous);
            instructions.Emit(OpCodes.Ldfld, ClrTypeFrom(arrayAccess.Array.Type, module).GetField("data"));
            
            Generate(arrayAccess.Position, scope, instructions, functionToBuilder, module);

            instructions.Emit(OpCodes.Conv_I);

            GenerateSizeOf(((ArrayType)arrayAccess.Array.Type).Of, instructions, module);
            instructions.Emit(OpCodes.Mul);
            instructions.Emit(OpCodes.Add);
        }
        else
            Report.InternalError($"Cannot translate {expression.GetType()} to IL.");
    }

    private static void GenerateSizeOf(Type type, ILGenerator instructions, ModuleBuilder module)
    {
        if (type is EnumerationDeclaration enumerationDeclaration)
            GenerateSizeOf(enumerationDeclaration.Type, instructions, module);
        else if (type == Semantics.I8 || type == Semantics.U8)
            instructions.Emit(OpCodes.Ldc_I4_1);
        else if (type == Semantics.I16 || type == Semantics.U16)
            instructions.Emit(OpCodes.Ldc_I4_2);
        else if (type == Semantics.I32 || type == Semantics.U32 || type == Semantics.F32)
            instructions.Emit(OpCodes.Ldc_I4_4);
        else if (type == Semantics.I64 || type == Semantics.U64 || type == Semantics.F64)
            instructions.Emit(OpCodes.Ldc_I4_8);
        else
            instructions.Emit(OpCodes.Sizeof, ClrTypeFrom(type, module));
    }

    private static Symbol Lookup(Identifier identifier, Scope scope)
    {
        foreach (var symbol in scope.Symbols)
        {
            if (symbol.Name == identifier.Value)
                return symbol;
        }

        if (scope.Parent != null)
            return Lookup(identifier, scope.Parent);
        else
            Report.InternalError($"{identifier.Start} {identifier.Value} is not in scope.");

        return null;
    }

    private static ClrType ClrTypeFrom(Type type, ModuleBuilder module)
    {
        if (type == null)
            return typeof(void);
        else if (type == Semantics.I8)
            return typeof(sbyte);
        else if (type == Semantics.I16)
            return typeof(short);
        else if (type == Semantics.I32)
            return typeof(int);
        else if (type == Semantics.I64)
            return typeof(long);
        else if (type == Semantics.U8)
            return typeof(byte);
        else if (type == Semantics.U16)
            return typeof(ushort);
        else if (type == Semantics.U32)
            return typeof(uint);
        else if (type == Semantics.U64)
            return typeof(ulong);
        else if (type == Semantics.F32)
            return typeof(float);
        else if (type == Semantics.F64)
            return typeof(double);
        else if (type == Semantics.Bool)
            return typeof(bool);
        else if (type is EnumerationDeclaration enumerationDeclaration)
        {
            var existingType = module.GetType(enumerationDeclaration.Name.Value);
            if (existingType != null)
                return existingType;

            var newType = module.DefineEnum(enumerationDeclaration.Name.Value, TypeAttributes.Public, ClrTypeFrom(enumerationDeclaration.Type, module));
            foreach (var constant in enumerationDeclaration.Constants)
            {
                if (type == Semantics.I8)
                    newType.DefineLiteral(constant.Name.Value, sbyte.Parse(constant.Value.Text));
                else if (type == Semantics.I16)
                    newType.DefineLiteral(constant.Name.Value, short.Parse(constant.Value.Text));
                else if (type == Semantics.I32)
                    newType.DefineLiteral(constant.Name.Value, int.Parse(constant.Value.Text));
                else if (type == Semantics.I64)
                    newType.DefineLiteral(constant.Name.Value, long.Parse(constant.Value.Text));
                else if (type == Semantics.U8)
                    newType.DefineLiteral(constant.Name.Value, byte.Parse(constant.Value.Text));
                else if (type == Semantics.U16)
                    newType.DefineLiteral(constant.Name.Value, ushort.Parse(constant.Value.Text));
                else if (type == Semantics.U32)
                    newType.DefineLiteral(constant.Name.Value, uint.Parse(constant.Value.Text));
                else if (type == Semantics.U64)
                    newType.DefineLiteral(constant.Name.Value, ulong.Parse(constant.Value.Text));
                else
                    Report.InternalError($"Cannot translate {type} to an IL enum constant.");
            }

            return newType.CreateType();
        }
        else if (type is CompoundDeclaration compoundDeclaration)
        {
            var existingType = module.GetType(compoundDeclaration.Name.Value);
            if (existingType != null)
                return existingType;

            var attributes = TypeAttributes.Public | TypeAttributes.Sealed;
            if (compoundDeclaration.Tag == CompoundTypeTag.Union)
                attributes |= TypeAttributes.ExplicitLayout;

            var newType = module.DefineType(compoundDeclaration.Name.Value, attributes, typeof(ValueType));
            foreach (var field in compoundDeclaration.Fields)
            {
                var f = newType.DefineField(field.Name.Value, ClrTypeFrom(field.Type, module), FieldAttributes.Public);
                if (compoundDeclaration.Tag == CompoundTypeTag.Union)
                    f.SetOffset(0);
            }

            return newType.CreateType();
        }
        else if (type is TupleType tupleType)
        {
            var types = tupleType.Types.Select(t => ClrTypeFrom(t, module)).ToArray();
            if (types.Length == 2)
                return typeof(ValueTuple<,>).MakeGenericType(types);
            else if (types.Length == 3)
                return typeof(ValueTuple<,,>).MakeGenericType(types);
            else if (types.Length == 4)
                return typeof(ValueTuple<,,,>).MakeGenericType(types);
            else if (types.Length == 5)
                return typeof(ValueTuple<,,,,>).MakeGenericType(types);
            else if (types.Length == 6)
                return typeof(ValueTuple<,,,,,>).MakeGenericType(types);
            else if (types.Length == 7)
                return typeof(ValueTuple<,,,,,,>).MakeGenericType(types);
            else
                Report.InternalError("Cannot translate 8 or more return values to IL.");
        }
        else if (type is ArrayType arrayType)
        {
            var genericType = module.GetType("_Array");
            if (genericType == null)
            {
                var builder = module.DefineType("_Array", TypeAttributes.Public | TypeAttributes.SequentialLayout | TypeAttributes.Sealed, typeof(ValueType));
                var T = builder.DefineGenericParameters("T")[0];
                builder.DefineField("length", typeof(uint), FieldAttributes.Public);
                builder.DefineField("capacity", typeof(uint), FieldAttributes.Public);
                builder.DefineField("data", T.MakePointerType(), FieldAttributes.Public);

                genericType = builder.CreateType();
            }

            return genericType.MakeGenericType(ClrTypeFrom(arrayType.Of, module));
        }
        else if (type is PointerType pointerType)
        {
            if (pointerType.To is UnresolvedType)
                return typeof(void*);
            else
                return ClrTypeFrom(pointerType.To, module).MakePointerType();
        }
        else
            Report.InternalError($"Cannot translate {type} to IL.");

        return null;
    }

    private static void LoadAddress(Symbol symbol, ILGenerator instructions)
    {
        if (symbol is LocalSymbol localSymbol)
            LoadLocalAddress(localSymbol.Index, instructions);
        else if (symbol is InputSymbol inputSymbol)
        {
            if (inputSymbol.Index > byte.MaxValue)
                instructions.Emit(OpCodes.Ldarga, inputSymbol.Index);
            else
                instructions.Emit(OpCodes.Ldarga_S, (byte)inputSymbol.Index);
        }
    }

    private static void LoadLocalAddress(ushort index, ILGenerator instructions)
    {
        if (index > byte.MaxValue)
            instructions.Emit(OpCodes.Ldloca, index);
        else
            instructions.Emit(OpCodes.Ldloca_S, (byte)index);
    }

    private static void LoadValue(Symbol symbol, ILGenerator instructions)
    {
        if (symbol is LocalSymbol localSymbol)
            LoadLocal(localSymbol.Index, instructions);
        else if (symbol is InputSymbol inputSymbol)
        {
            if (inputSymbol.Index == 0)
                instructions.Emit(OpCodes.Ldarg_0);
            else if (inputSymbol.Index == 1)
                instructions.Emit(OpCodes.Ldarg_1);
            else if (inputSymbol.Index == 2)
                instructions.Emit(OpCodes.Ldarg_2);
            else if (inputSymbol.Index == 3)
                instructions.Emit(OpCodes.Ldarg_3);
            else if (inputSymbol.Index > byte.MaxValue)
                instructions.Emit(OpCodes.Ldarg, inputSymbol.Index);
            else
                instructions.Emit(OpCodes.Ldarg_S, (byte)inputSymbol.Index);
        }
    }

    private static void LoadLocal(ushort index, ILGenerator instructions)
    {
        if (index == 0)
            instructions.Emit(OpCodes.Ldloc_0);
        else if (index == 1)
            instructions.Emit(OpCodes.Ldloc_1);
        else if (index == 2)
            instructions.Emit(OpCodes.Ldloc_2);
        else if (index == 3)
            instructions.Emit(OpCodes.Ldloc_3);
        else if (index > byte.MaxValue)
            instructions.Emit(OpCodes.Ldloc, index);
        else
            instructions.Emit(OpCodes.Ldloc_S, (byte)index);
    }
}