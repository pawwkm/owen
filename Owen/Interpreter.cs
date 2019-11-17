﻿using System;
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
            if (expression is Call call)
            {
                var stack = new List<Variable>();
                for (var i = 0; i < call.Arguments.Count; i++)
                {
                    stack.Add(new Variable()
                    {
                        Name = call.DeclarationOfCallee.Input[i].Name.Value,
                        Value = Run(call.Arguments[i], variables)
                    });
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
                        return Run(r.Expressions[0], variables);
                    else
                        throw new NotImplementedException("Cannot interpret multiple return values.");
                }
            }

            return null;
        }
    }
}