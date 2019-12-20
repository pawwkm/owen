using System;
using System.Collections.Generic;
using System.Linq;

namespace Owen
{
    internal static class Semantics
    {
        private static readonly PrimitiveType I8 = new PrimitiveType() { Tag = PrimitiveTypeTag.I8 };
        private static readonly PrimitiveType I16 = new PrimitiveType() { Tag = PrimitiveTypeTag.I16 };
        private static readonly PrimitiveType I32 = new PrimitiveType() { Tag = PrimitiveTypeTag.I32 };
        private static readonly PrimitiveType I64 = new PrimitiveType() { Tag = PrimitiveTypeTag.I64 };
        private static readonly PrimitiveType U8 = new PrimitiveType() { Tag = PrimitiveTypeTag.U8 };
        private static readonly PrimitiveType U16 = new PrimitiveType() { Tag = PrimitiveTypeTag.U16 };
        private static readonly PrimitiveType U32 = new PrimitiveType() { Tag = PrimitiveTypeTag.U32 };
        private static readonly PrimitiveType U64 = new PrimitiveType() { Tag = PrimitiveTypeTag.U64 };
        private static readonly PrimitiveType F32 = new PrimitiveType() { Tag = PrimitiveTypeTag.F32 };
        private static readonly PrimitiveType F64 = new PrimitiveType() { Tag = PrimitiveTypeTag.F64 };
        private static readonly PrimitiveType Bool = new PrimitiveType() { Tag = PrimitiveTypeTag.Bool };

        public static void Analyze(Program program)
        {
            program.Scope.Symbols.Add(new Symbol() { Name = I8.Tag.ToString(), Type = I8 });
            program.Scope.Symbols.Add(new Symbol() { Name = I16.Tag.ToString(), Type = I16 });
            program.Scope.Symbols.Add(new Symbol() { Name = I32.Tag.ToString(), Type = I32 });
            program.Scope.Symbols.Add(new Symbol() { Name = I64.Tag.ToString(), Type = I64 });
            program.Scope.Symbols.Add(new Symbol() { Name = U8.Tag.ToString(), Type = U8 });
            program.Scope.Symbols.Add(new Symbol() { Name = U16.Tag.ToString(), Type = U16 });
            program.Scope.Symbols.Add(new Symbol() { Name = U32.Tag.ToString(), Type = U32 });
            program.Scope.Symbols.Add(new Symbol() { Name = U64.Tag.ToString(), Type = U64 });
            program.Scope.Symbols.Add(new Symbol() { Name = F32.Tag.ToString(), Type = F32 });
            program.Scope.Symbols.Add(new Symbol() { Name = F64.Tag.ToString(), Type = F64 });
            program.Scope.Symbols.Add(new Symbol() { Name = Bool.Tag.ToString(), Type = Bool });

            foreach (var file in program.Files)
                Analyze(file, program);

            var mainFunctions = program.Files.SelectMany(f => f.Functions)
                                             .Where(f => f.Name.Value == "main")
                                             .ToList();

            if (mainFunctions.Count == 0)
                Report.Error("No main function defined.");
            else if (mainFunctions.Count > 1)
                Report.Error($"Multiple main functons defined:\r\n{string.Join("\r\n", mainFunctions.Select(f => f.Name.Start))}");

            var main = mainFunctions[0];
            if (!Compare(main.Output, I32))
                Report.Error($"{main.Name.Start} main must output a single {I32}.");
        }

        private static void Analyze(File file, Program program)
        {
            file.Scope = new Scope();
            file.Scope.Parent = program.Scope;

            foreach (var function in file.Functions)
            {
                for (var i = 0; i < function.Input.Count; i++)
                    function.Input[i].Type = Lookup(file.Scope, ((UnresolvedType)function.Input[i].Type).Identifier);

                if (function.Output is TupleType tuple)
                {
                    for (var i = 0; i < tuple.Types.Count; i++)
                        tuple.Types[i] = Lookup(file.Scope, ((UnresolvedType)tuple.Types[i]).Identifier);
                }
                else if (function.Output != null)
                    function.Output = Lookup(file.Scope, ((UnresolvedType)function.Output).Identifier);

                file.Scope.Symbols.Add(new Symbol()
                {
                    Name = function.Name.Value,
                    Type = function
                });
            }

            foreach (var function in file.Functions)
            {
                function.Body.Scope.Parent = file.Scope;
                var index = default(ushort);

                foreach (var argument in function.Input)
                {
                    if (NullableLookup(function.Body.Scope, argument.Name.Value) is PrimitiveType primitive)
                        Report.Error($"{argument.Name.Start} Redeclares {argument.Name.Value}.");
                    else
                    {
                        function.Body.Scope.Symbols.Add(new InputSymbol()
                        {
                            Name = argument.Name.Value,
                            Type = argument.Type,
                            Index = index++
                        });
                    }
                }

                Analyze(function.Body, function.Output, function.Body.Scope);
            }
        }

        private static void Analyze(CompoundStatement compound, Type output, Scope parent)
        {
            foreach (var statement in compound.Statements)
            {
                if (statement is AssignmentStatement assignment)
                {
                    var locals = CountLocals(compound.Scope);
                    void Mismatch(int left, int right) =>
                        Report.Error($"{assignment.Operator.Start} Assignment mismatch of {left} variable{(left > 1 ? "s" : "")} and {right} output{(right > 1 ? "s" : "")}.");

                    Expression Assign(Expression left, Type rightType, Position right)
                    {
                        if (left is Identifier reference)
                        {
                            var leftType = NullableLookup(compound.Scope, reference.Value);
                            if (leftType == null)
                            {
                                if (leftType == null && rightType == null)
                                    Report.Error($"{right} Cannot infer type of expression.");

                                if (assignment.Operator.Tag != OperatorTag.Equal)
                                    Report.Error($"{reference.Start} Undefined reference to {reference.Value}.");
                                else
                                {
                                    left = new VariableDeclaration()
                                    {
                                        Type = rightType,
                                        Variable = reference
                                    };

                                    compound.Scope.Symbols.Add(new LocalSymbol()
                                    {
                                        Name = reference.Value,
                                        Type = rightType,
                                        Index = locals++
                                    });
                                }
                            }
                            else if (!Compare(leftType, rightType))
                                Report.Error($"{right} Expected {leftType} but found {rightType}.");
                        }
                        else
                            Report.Error($"{left.Start} The expression is not addressable.");

                        var operatorUsedOnNonInteger = (assignment.Operator.Tag == OperatorTag.BitwiseAndEqual  ||
                                                        assignment.Operator.Tag == OperatorTag.BitwiseOrEqual   ||
                                                        assignment.Operator.Tag == OperatorTag.BitwiseXorEqual  ||
                                                        assignment.Operator.Tag == OperatorTag.LeftShiftEqual   ||
                                                        assignment.Operator.Tag == OperatorTag.RightShiftEqual) &&
                                                      (!(rightType is PrimitiveType) ||
                                                      ((PrimitiveType)rightType).Tag > PrimitiveTypeTag.U64);

                        if (operatorUsedOnNonInteger)
                            Report.Error($"{assignment.Operator.Start} This operator is only defined for integer types.");

                        return left;
                    }

                    for (var r = 0; r < assignment.Right.Count; r++)
                    {
                        var rightType = default(Type);
                        if (assignment.Right[r] is Call call)
                        {
                            rightType = Analyze(assignment.Right[r], null, compound.Scope);
                            if (rightType is TupleType tuple)
                            {
                                if (assignment.Right.Count != 1)
                                    Report.Error($"{call.Start} A Call outputting multiple values must be the only expression on the right hand side.");
                                else if (assignment.Left.Count != tuple.Types.Count)
                                    Mismatch(assignment.Left.Count, tuple.Types.Count);
                                else
                                {
                                    for (var l = 0; l < assignment.Left.Count; l++)
                                        assignment.Left[l] = Assign(assignment.Left[l], tuple.Types[l], call.Start);
                                }

                                break;
                            }
                        }
                        else if (assignment.Left.Count != assignment.Right.Count)
                            Mismatch(assignment.Left.Count, assignment.Right.Count);
                        else if (rightType == null && assignment.Left[r] is Identifier reference)
                        {
                            rightType = NullableLookup(compound.Scope, reference.Value);
                            if (rightType == null)
                                rightType = Analyze(assignment.Right[r], null, compound.Scope);
                            else
                                rightType = Analyze(assignment.Right[r], rightType, compound.Scope);
                        }

                        assignment.Left[r] = Assign(assignment.Left[r], rightType, assignment.Right[r].Start); 
                    }
                }
                else if (statement is ExpressionStatement e)
                {
                    Analyze(e.Expression, null, compound.Scope);
                    if (!(e.Expression is Call))
                        Report.Error($"{e.Expression.Start} Call or pre/post increment/decrement expression expected.");
                }
                else if (statement is ReturnStatement r)
                {
                    var expressionsAndOutputIsNotBalanced = false;
                    if (output is TupleType t)
                        expressionsAndOutputIsNotBalanced = r.Expressions.Count != t.Types.Count;
                    else if (output == null && r.Expressions.Count != 0 || output != null && r.Expressions.Count != 1)
                        expressionsAndOutputIsNotBalanced = true;

                    if (expressionsAndOutputIsNotBalanced)
                        Report.Error($"{r.EndOfKeyword} The amount of return values doesn't match the output.");

                    if (output is TupleType tuple)
                    {
                        for (var i = 0; i < tuple.Types.Count; i++)
                        {
                            var expressionType = Analyze(r.Expressions[i], tuple.Types[i], compound.Scope);
                            if (expressionType != tuple.Types[i])
                                Report.Error($"{r.Expressions[i].Start} Expected {tuple.Types[i]} but found {expressionType}.");
                        }
                    }
                    else if (output != null)
                    {
                        var expressionType = Analyze(r.Expressions[0], output, compound.Scope);
                        if (expressionType != output)
                            Report.Error($"{r.Expressions[0].Start} Expected {output} but found {expressionType}.");
                    }
                }
                else if (statement is AssertStatement assert)
                {
                    var type = Analyze(assert.Assertion, null, compound.Scope);
                    if (!Compare(type, Bool))
                        Report.Error($"{assert.Assertion.Start} {Bool} expression expected.");
                }
                else
                    Report.Error($"Cannot analyze {statement.GetType().Name}.");
            }
        }

        public static Type Analyze(Expression expression, Type expectedType, Scope scope)
        {
            if (expression is BinaryExpression binary)
            {
                var leftType = Analyze(binary.Left, null, scope);
                var rightType = Analyze(binary.Right, leftType, scope);

                binary.Type = new PrimitiveType()
                {
                    Tag = PrimitiveTypeTag.Bool
                };

                if (!Compare(leftType, rightType))
                    Report.Error($"{binary.Right.Start} Expected {leftType} but found {rightType}.");
                else
                    return binary.Type;
            }
            else if (expression is Number number)
            {
                if (number.Tag == NumberTag.ToBeInfered)
                {
                    if (expectedType is PrimitiveType primitive)
                    {
                        number.Tag = (NumberTag)primitive.Tag;
                        number.Type = primitive;
                    }
                    else
                        return null;
                }
                else
                    number.Type = NullableLookup(scope, number.Tag.ToString());

                return number.Type;
            }
            else if (expression is Identifier reference)
            {
                reference.Type = Lookup(scope, reference);

                return reference.Type;
            }
            else if (expression is Call call)
            {
                if (Analyze(call.Reference, null, scope) is FunctionDeclaration function)
                {
                    if (call.Arguments.Count != function.Input.Count)
                        Report.Error($"{call.Reference.Start} No function overload fitting the given input.");

                    call.Declaration = function;
                    for (var i = 0; i < call.Arguments.Count; i++)
                    {
                        if (!Compare(function.Input[i].Type, Analyze(call.Arguments[i], function.Input[i].Type, scope)))
                            Report.Error($"{call.Reference.Start} No function overload fitting the given input.");
                    }

                    call.Type = function.Output;

                    return function.Output;
                }
                else
                    Report.Error($"{call.Reference.Start} Function expected.");
            }

            throw new NotImplementedException($"Cannot analyze {expression.GetType().Name}.");
        }

        private static Type Lookup(Scope scope, Identifier name)
        {
            var type = NullableLookup(scope, name.Value);
            if (type == null)
                Report.Error($"{name.Start} Undefined reference to {name.Value}.");

            return type;
        }

        private static Type NullableLookup(Scope scope, string name)
        {
            foreach (var symbol in scope.Symbols)
            {
                if (symbol.Name == name)
                    return symbol.Type;
            }

            if (scope.Parent == null)
                return null;
            else
                return NullableLookup(scope.Parent, name);
        }

        private static bool Compare(Type a, Type b)
        {
            if (a is PrimitiveType pa && b is PrimitiveType pb)
                return pa.Tag == pb.Tag;
            else if (a is TupleType ta && b is TupleType tb)
            {
                if (ta.Types.Count != tb.Types.Count)
                    return false;
                else
                {
                    for (var i = 0; i < ta.Types.Count; i++)
                    {
                        if (!Compare(ta.Types[i], tb.Types[i]))
                            return false;
                    }

                    return true;
                }
            }
            else
                return false;
        }

        private static ushort CountLocals(Scope scope)
        {
            ushort count = 0;
            foreach (var symbol in scope.Symbols)
            {
                if (symbol is LocalSymbol)
                    count++;
            }

            if (scope.Parent == null)
                return count;
            else
                return (ushort)(count + CountLocals(scope.Parent));
        }
    }
}