﻿using System;
using System.Collections.Generic;
using System.Linq;

namespace Owen
{
    internal static class Semantics
    {
        public static void Analyze(Program program)
        {
            foreach (PrimitiveTypeTag tag in Enum.GetValues(typeof(PrimitiveTypeTag)))
            {
                program.Scope.Symbols.Add(new Symbol()
                {
                    Name = tag.ToString().ToLower(),
                    Type = new PrimitiveType()
                    {
                        Tag = tag
                    }
                });
            }
            
            foreach (var file in program.Files)
                Analyze(file, program);

            var mainFunctions = program.Files.SelectMany(f => f.Functions)
                                             .Where(f => f.Name.Value == "main")
                                             .ToList();

            if (mainFunctions.Count == 0)
                Report.Error("No main function defined.");
            else if (mainFunctions.Count > 1)
                Report.Error($"Multiple main functons defined:\r\n{string.Join("\r\n", mainFunctions.Select(f => f.Name.DeclaredAt))}");

            var main = mainFunctions[0];
            if (main.Output.Count != 1 || main.Output[0].Value != "i32")
                Report.Error($"{main.Name.DeclaredAt} main must output a single i32.");
        }

        private static void Analyze(File file, Program program)
        {
            file.Scope = new Scope();
            file.Scope.Parent = program.Scope;

            foreach (var function in file.Functions)
            {
                var type = new FunctionType();
                type.Declaration = function;
                type.Input.AddRange(function.Input.Select(o => Lookup(file.Scope, o.Type)));
                type.Output.AddRange(function.Output.Select(o => Lookup(file.Scope, o.Value)));

                file.Scope.Symbols.Add(new Symbol()
                {
                    Name = function.Name.Value, 
                    Type = type
                });
            }

            foreach (var function in file.Functions)
            {
                function.Body.Scope.Parent = file.Scope;
                foreach (var argument in function.Input)
                {
                    var asd = Lookup(function.Body.Scope, argument.Name.Value);
                    if (asd is PrimitiveType primitive)
                        Report.Error($"{argument.Name.DeclaredAt} Redeclares {argument.Name.Value}.");
                    else
                    {
                        function.Body.Scope.Symbols.Add(new Symbol() 
                        { 
                            Name = argument.Name.Value,
                            Type = Lookup(function.Body.Scope, argument.Type)
                        });
                    }
                }

                Analyze(function.Body, function.Output, function.Body.Scope);
            }
        }

        private static void Analyze(CompoundStatement compound, List<Identifier> output, Scope parent)
        {
            foreach (var statement in compound.Statements)
            {
                if (statement is AssignmentStatement assignment)
                {
                    if (assignment.Left.Count > assignment.Right.Count)
                        Report.Error($"{PositionOf(assignment.Left.Last())} No expression is being stored.");
                    else if (assignment.Left.Count < assignment.Right.Count)
                        Report.Error($"{PositionOf(assignment.Right.Last())} No variable to store the value in.");
                    else
                    {
                        for (var i = 0; i < assignment.Left.Count; i++)
                        {
                            // Test that both operands are primitives!
                            var left = assignment.Left[i];
                            var leftType = default(Type);

                            if (left is Identifier reference)
                            {
                                var right = assignment.Right[i];
                                var inferred = Lookup(compound.Scope, reference.Value);

                                if (inferred == null)
                                {
                                    leftType = Analyze(right, null, compound.Scope);
                                    if (leftType == null)
                                        Report.Error($"{PositionOf(right)} Cannot infer type of expression.");
                                    else
                                    {
                                        assignment.Left[i] = new VariableDeclaration()
                                        {
                                            Type = leftType,
                                            Variable = reference
                                        };

                                        compound.Scope.Symbols.Add(new Symbol()
                                        {
                                            Name = reference.Value,
                                            Type = leftType
                                        });
                                    }
                                }
                                else
                                {
                                    var rightType = Analyze(right, inferred, compound.Scope);
                                    leftType = Analyze(left, rightType, compound.Scope);

                                    if (!Compare(rightType, leftType))
                                        Report.Error($"{PositionOf(right)} Expected {leftType} but found {rightType}.");
                                }
                            }
                            else
                                Report.Error($"{PositionOf(left)} The expression is not addressable.");

                            switch (assignment.Operator.Tag)
                            {
                                case OperatorTag.BitwiseAndEqual:
                                case OperatorTag.BitwiseOrEqual:
                                case OperatorTag.BitwiseXorEqual:
                                case OperatorTag.LeftShiftEqual:
                                case OperatorTag.RightShiftEqual:
                                    if (Compare(leftType, Lookup(parent, "f32")) || Compare(leftType, Lookup(parent, "f64")))
                                        Report.Error($"{assignment.Operator.DefinedAt} This operator is only defined for integer types.");
                                    break;
                            }
                        }
                    }
                }
                else if (statement is ReturnStatement r)
                    Analyze(r, output, compound.Scope);
                else
                    throw new NotImplementedException($"Cannot analyze {statement.GetType().Name}.");
            }
        }

        private static void Analyze(ReturnStatement statement, List<Identifier> output, Scope scope)
        {
            if (output.Count != statement.Expressions.Count)
                Report.Error($"{statement.EndOfKeyword} The amount of return values doesn't match the output.");

            for (var i = 0; i < output.Count; i++)
            {
                var outputType = Lookup(scope, output[i]);
                var expressionType = Analyze(statement.Expressions[i], outputType, scope);

                if (expressionType != outputType)
                    Report.Error($"{PositionOf(statement.Expressions[i])} Expected {outputType} but found {expressionType}.");
            }
        }

        public static Type Analyze(Expression expression, Type expectedType, Scope scope)
        {
            if (expression is Number number)
            {
                if (number.Tag == NumberTag.IntegerToBeInfered)
                {
                    if (expectedType is PrimitiveType primitive)
                        number.Tag = (NumberTag)(primitive.Tag);
                    else
                        return null;
                }

                return Lookup(scope, number.Tag.ToString().ToLower());
            }
            else if (expression is Identifier reference)
                return Lookup(scope, reference);
            else if (expression is Call call)
            {
                if (Analyze(call.Callee, null, scope) is FunctionType type)
                {
                    if (call.Arguments.Count != type.Input.Count)
                        Report.Error($"{PositionOf(call.Callee)} No function overload fitting the given input.");

                    for (var i = 0; i < call.Arguments.Count; i++)
                    {
                        if (!Compare(type.Input[i], Analyze(call.Arguments[i], type.Input[i], scope)))
                            Report.Error($"{PositionOf(call.Callee)} No function overload fitting the given input.");
                    }

                    call.DeclarationOfCallee = type.Declaration;
                    if (type.Output.Count == 0)
                        return null;
                    else if (type.Output.Count == 1)
                        return type.Output[0];
                    else
                        throw new NotImplementedException($"Multiple return values not yet implemented.");
                }
                else
                    Report.Error($"{PositionOf(call.Callee)} Function expected.");
            }

            throw new NotImplementedException($"Cannot analyze {expression.GetType().Name}.");
        }

        private static Type Lookup(Scope scope, Identifier name)
        {
            var type = Lookup(scope, name.Value);
            if (type == null)
                Report.Error($"{name.DeclaredAt} Undefined reference to {name.Value}.");

            return type;
        }

        private static Type Lookup(Scope scope, string name)
        {
            foreach (var symbol in scope.Symbols)
            {
                if (symbol.Name == name)
                    return symbol.Type;
            }

            if (scope.Parent == null)
                return null;
            else
                return Lookup(scope.Parent, name);
        }

        private static bool Compare(Type a, Type b)
        {
            if (a is PrimitiveType pa && b is PrimitiveType pb)
                return pa.Tag == pb.Tag;
            else
                return false;
        }

        private static Position PositionOf(Expression expression)
        {
            if (expression is Number number)
                return number.DeclaredAt;
            else if (expression is Identifier reference)
                return reference.DeclaredAt;
            else
                throw new NotImplementedException($"Cannot find position of {expression.GetType().Name}.");
        }
    }
}