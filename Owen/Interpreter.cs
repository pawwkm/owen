using System;
using System.Collections.Generic;

namespace Owen
{
    internal static class Interpreter
    {
        private class Variable
        {
            public string Name;
            public dynamic Value;
        }

        public static void Run(ref Expression expression)
        {
            var lo = Run(ref expression, new List<Variable>());
            if (lo is sbyte)
                expression = new Number()
                {
                    Tag = NumberTag.I8,
                    Value = lo.ToString()
                };
            else if (lo is short)
                expression = new Number()
                {
                    Tag = NumberTag.I16,
                    Value = lo.ToString()
                };
            else if (lo is int)
                expression = new Number()
                {
                    Tag = NumberTag.I32,
                    Value = lo.ToString()
                };
            else if (lo is long)
                expression = new Number()
                {
                    Tag = NumberTag.I64,
                    Value = lo.ToString()
                };
            else if (lo is byte)
                expression = new Number()
                {
                    Tag = NumberTag.U8,
                    Value = lo.ToString()
                };
            else if (lo is ushort)
                expression = new Number()
                {
                    Tag = NumberTag.U16,
                    Value = lo.ToString()
                };
            else if (lo is uint)
                expression = new Number()
                {
                    Tag = NumberTag.U32,
                    Value = lo.ToString()
                };
            else if (lo is ulong)
                expression = new Number()
                {
                    Tag = NumberTag.U64,
                    Value = lo.ToString()
                };
            else
                throw new NotImplementedException($"Cannot rewrite expression to type of {lo.GetType().Name}.");
        }

        private static dynamic Run(ref Expression expression, List<Variable> variables)
        {
            if (expression is Call call)
            {
                var stack = new List<Variable>();
                for (var i = 0; i < call.Arguments.Count; i++)
                {
                    var argument = call.Arguments[i];
                    stack.Add(new Variable()
                    {
                        Name = call.DeclarationOfCallee.Input[i].Name.Value,
                        Value = Run(ref argument, variables)
                    });

                    call.Arguments[i] = argument;
                }

                return Run(call.DeclarationOfCallee.Body, stack);
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
                if (statement is ReturnStatement r)
                {
                    if (r.Expressions.Count == 0)
                        return null;
                    else if (r.Expressions.Count == 1)
                    {
                        var expression = r.Expressions[0];

                        return Run(ref expression, variables);
                    }
                    else
                        throw new NotImplementedException("Cannot interpret multiple return values.");
                }
            }

            return null;
        }
    }
}