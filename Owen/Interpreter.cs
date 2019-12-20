using System;
using System.Collections.Generic;
using System.Linq;

namespace Owen
{
    internal static class Interpreter
    {
        private class Variable
        {
            public string Name;
            public dynamic Value;
        }

        public static void Run(Program program)
        {
            foreach (var file in program.Files)
            {
                foreach (var expression in file.Ctfe)
                {
                    Semantics.Analyze(expression, null, file.Scope);
                    Run(expression, new List<Variable>());
                }
            }
        }

        private static dynamic Run(Expression expression, List<Variable> variables)
        {
            if (expression is BinaryExpression binary)
            {
                switch (binary.Operator.Tag)
                {
                    case OperatorTag.EqualEqual:
                        return Run(binary.Left, variables) == Run(binary.Right, variables);
                    case OperatorTag.NotEqual:
                        return Run(binary.Left, variables) != Run(binary.Right, variables);
                    default:
                        Report.Error($"{binary.Operator.Start} Cannot interpret the {binary.Operator.Tag} operator.");
                        return null;
                }
            }
            else if (expression is Call call)
            {
                var input = new List<Variable>();
                for (var i = 0; i < call.Arguments.Count; i++)
                {
                    input.Add(new Variable()
                    {
                        Name = call.Declaration.Input[i].Name.Value,
                        Value = Run(call.Arguments[i], variables)
                    });
                }

                return Run(call.Declaration.Body, input);
            }
            else if (expression is Number number)
            {
                switch (number.Tag)
                {
                    case NumberTag.I8:
                        return sbyte.Parse(number.Value);
                    case NumberTag.I16:
                        return short.Parse(number.Value);
                    case NumberTag.I32:
                        return int.Parse(number.Value);
                    case NumberTag.I64:
                        return long.Parse(number.Value);
                    case NumberTag.U8:
                        return byte.Parse(number.Value);
                    case NumberTag.U16:
                        return ushort.Parse(number.Value);
                    case NumberTag.U32:
                        return uint.Parse(number.Value);
                    case NumberTag.U64:
                        return ulong.Parse(number.Value);
                    case NumberTag.F32:
                        return float.Parse(number.Value);
                    case NumberTag.F64:
                        return double.Parse(number.Value);
                    default:
                        throw new NotImplementedException($"Cannot interpret {expression.GetType().Name}");
                }
            }
            else if (expression is Identifier reference)
                return variables.Find(v => v.Name == reference.Value).Value;
            else
                throw new NotImplementedException($"Cannot interpret {expression.GetType().Name}");
        }

        private static dynamic Run(CompoundStatement compound, List<Variable> variables)
        {
            foreach (var statement in compound.Statements)
            {
                if (statement is AssignmentStatement assignment)
                {
                    var l = 0;
                    foreach (var expression in assignment.Right.Select(r => Run(r, variables)))
                    {
                        void Assign(Expression left, dynamic right)
                        {
                            if (left is VariableDeclaration declaration)
                            {
                                variables.Add(new Variable()
                                {
                                    Name = declaration.Variable.Value,
                                    Value = right
                                });
                            }
                            else if (left is Identifier reference)
                            {
                                var variable = variables.First(v => v.Name == reference.Value);
                                var type = variable.Value.GetType();

                                switch (assignment.Operator.Tag)
                                {
                                    case OperatorTag.PlusEqual:
                                        variable.Value += right;
                                        break;
                                    case OperatorTag.MinusEqual:
                                        variable.Value -= right;
                                        break;
                                    case OperatorTag.MultiplyEqual:
                                        variable.Value *= right;
                                        break;
                                    case OperatorTag.DivideEqual:
                                        variable.Value /= right;
                                        break;
                                    case OperatorTag.ModuloEqual:
                                        variable.Value %= right;
                                        break;
                                    case OperatorTag.BitwiseAndEqual:
                                        variable.Value &= right;
                                        break;
                                    case OperatorTag.BitwiseOrEqual:
                                        variable.Value |= right;
                                        break;
                                    case OperatorTag.BitwiseXorEqual:
                                        variable.Value ^= right;
                                        break;
                                    case OperatorTag.LeftShiftEqual:
                                        if (type == typeof(sbyte) || type == typeof(short) || type == typeof(int) || type == typeof(long))
                                            variable.Value = (long)variable.Value;
                                        else
                                            variable.Value = (ulong)variable.Value;

                                        variable.Value <<= (int)right;
                                        break;
                                    case OperatorTag.RightShiftEqual:
                                        if (type == typeof(sbyte) || type == typeof(short) || type == typeof(int) || type == typeof(long))
                                            variable.Value = (long)variable.Value;
                                        else
                                            variable.Value = (ulong)variable.Value;

                                        variable.Value >>= (int)right;
                                        break;
                                    case OperatorTag.Equal:
                                        variable.Value = right;
                                        break;
                                }

                                if (type != variable.Value.GetType())
                                {
                                    if (type == typeof(sbyte))
                                        variable.Value = (sbyte)variable.Value;
                                    else if (type == typeof(short))
                                        variable.Value = (short)variable.Value;
                                    else if (type == typeof(int))
                                        variable.Value = (int)variable.Value;
                                    else if (type == typeof(long))
                                        variable.Value = (long)variable.Value;
                                    else if (type == typeof(byte))
                                        variable.Value = (byte)variable.Value;
                                    else if (type == typeof(ushort))
                                        variable.Value = (ushort)variable.Value;
                                    else if (type == typeof(uint))
                                        variable.Value = (uint)variable.Value;
                                    else if (type == typeof(ulong))
                                        variable.Value = (ulong)variable.Value;
                                    else if (type == typeof(float))
                                        variable.Value = (float)variable.Value;
                                    else if (type == typeof(double))
                                        variable.Value = (double)variable.Value;
                                }
                            }
                            else
                                throw new NotImplementedException($"Cannot interpret assignment to {left.GetType().Name}.");

                            l++;
                        }

                        if (expression is Array)
                        {
                            for (var i = 0; i < expression.Length; i++)
                                Assign(assignment.Left[l], expression[i]);
                        }
                        else
                            Assign(assignment.Left[l], expression);
                    }
                }
                else if (statement is ExpressionStatement e)
                    Run(e.Expression, variables);
                else if (statement is ReturnStatement r)
                {
                    var values = new dynamic[r.Expressions.Count];
                    for (var i = 0; i < values.Length; i++)
                        values[i] = Run(r.Expressions[i], variables);

                    return values;
                }
                else if (statement is AssertStatement assert)
                {
                    if (!Run(assert.Assertion, variables))
                        Report.Error($"{assert.Assertion.Start} Assertion failed.");
                }
                else
                    throw new NotImplementedException($"Cannot interpret {statement.GetType().Name}.");
            }

            return null;
        }
    }
}