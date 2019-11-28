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
            if (main.Output.Count != 1 || !Compare(main.Output[0], I32))
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

                for (var i = 0; i < function.Output.Count; i++)
                    function.Output[i] = Lookup(file.Scope, ((UnresolvedType)function.Output[i]).Identifier);

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

        private static void Analyze(CompoundStatement compound, List<Type> output, Scope parent)
        {
            foreach (var statement in compound.Statements)
            {
                if (statement is AssignmentStatement assignment)
                {
                    if (assignment.Left.Count > assignment.Right.Count)
                        Report.Error($"{assignment.Left.Last().Start} No expression is being stored.");
                    else if (assignment.Left.Count < assignment.Right.Count)
                        Report.Error($"{assignment.Right.Last().Start} No variable to store the value in.");
                    else
                    {
                        var locals = CountLocals(compound.Scope);
                        for (var i = 0; i < assignment.Left.Count; i++)
                        {
                            var left = assignment.Left[i];
                            var leftType = default(Type);

                            if (left is Identifier reference)
                            {
                                var right = assignment.Right[i];
                                var inferred = NullableLookup(compound.Scope, reference.Value);

                                if (inferred == null)
                                {
                                    leftType = Analyze(right, null, compound.Scope);
                                    if (leftType == null)
                                        Report.Error($"{right.Start} Cannot infer type of expression.");
                                    else
                                    {
                                        assignment.Left[i] = new VariableDeclaration()
                                        {
                                            Type = leftType,
                                            Variable = reference
                                        };

                                        compound.Scope.Symbols.Add(new LocalSymbol()
                                        {
                                            Name = reference.Value,
                                            Type = leftType,
                                            Index = locals++
                                        });
                                    }
                                }
                                else
                                {
                                    var rightType = Analyze(right, inferred, compound.Scope);
                                    leftType = Analyze(left, rightType, compound.Scope);

                                    if (!Compare(rightType, leftType))
                                        Report.Error($"{right.Start} Expected {leftType} but found {rightType}.");
                                }
                            }
                            else
                                Report.Error($"{left.Start} The expression is not addressable.");

                            var operatorUsedOnNonInteger = (assignment.Operator.Tag == OperatorTag.BitwiseAndEqual ||
                                                            assignment.Operator.Tag == OperatorTag.BitwiseOrEqual ||
                                                            assignment.Operator.Tag == OperatorTag.BitwiseXorEqual ||
                                                            assignment.Operator.Tag == OperatorTag.LeftShiftEqual ||
                                                            assignment.Operator.Tag == OperatorTag.RightShiftEqual) &&
                                                          (!(leftType is PrimitiveType) ||
                                                          ((PrimitiveType)leftType).Tag > PrimitiveTypeTag.U64);
                            if (operatorUsedOnNonInteger)
                                Report.Error($"{assignment.Operator.Start} This operator is only defined for integer types.");
                        }
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
                    if (output.Count != r.Expressions.Count)
                        Report.Error($"{r.EndOfKeyword} The amount of return values doesn't match the output.");

                    for (var i = 0; i < output.Count; i++)
                    {
                        var expressionType = Analyze(r.Expressions[i], output[i], compound.Scope);
                        if (expressionType != output[i])
                            Report.Error($"{r.Expressions[i].Start} Expected {output[i]} but found {expressionType}.");
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

                if (!Compare(leftType, rightType))
                    Report.Error($"{binary.Right.Start} Expected {leftType} but found {rightType}.");
                else
                    return new PrimitiveType()
                    {
                        Tag = PrimitiveTypeTag.Bool
                    };
            }
            else if (expression is Number number)
            {
                if (number.Tag == NumberTag.ToBeInfered)
                {
                    if (expectedType is PrimitiveType primitive)
                        number.Tag = (NumberTag)primitive.Tag;
                    else
                        return null;
                }

                return NullableLookup(scope, number.Tag.ToString());
            }
            else if (expression is Identifier reference)
                return Lookup(scope, reference);
            else if (expression is Call call)
            {
                if (Analyze(call.Reference, null, scope) is FunctionDeclaration function)
                {
                    if (call.Arguments.Count != function.Input.Count)
                        Report.Error($"{call.Reference.Start} No function overload fitting the given input.");

                    for (var i = 0; i < call.Arguments.Count; i++)
                    {
                        if (!Compare(function.Input[i].Type, Analyze(call.Arguments[i], function.Input[i].Type, scope)))
                            Report.Error($"{call.Reference.Start} No function overload fitting the given input.");
                    }

                    call.Declaration = function;
                    if (function.Output.Count == 0)
                        return null;
                    else if (function.Output.Count == 1)
                        return function.Output[0];
                    else
                        throw new NotImplementedException($"Multiple return values not yet implemented.");
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