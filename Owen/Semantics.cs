using System;
using System.Collections.Generic;
using System.Linq;

namespace Owen
{
    internal static class Semantics
    {
        public static void Analyze(Program program)
        {
            foreach (var file in program.Files)
                Analyze(file);

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

        private static void Analyze(File file)
        {
            foreach (var function in file.Functions)
                Analyze(function);
        }

        private static void Analyze(FunctionDeclaration function)
        {
            Analyze(function.Body, function.Output);
        }

        private static void Analyze(CompoundStatement compound, List<Identifier> output)
        {
            foreach (var statement in compound.Statements)
            {
                if (statement is ReturnStatement r)
                    Analyze(r, output);
                else
                    throw new NotImplementedException($"Cannot analyze {statement.GetType().Name}.");
            }
        }

        private static void Analyze(ReturnStatement statement, List<Identifier> output)
        {
            if (output.Count != statement.Expressions.Count)
                Report.Error($"{statement.EndOfKeyword} The amount of return values doesn't match the output.");

            for (var i = 0; i < output.Count; i++)
            {
                var type = Analyze(statement.Expressions[i], output[i].Value);
                if (type != output[i].Value)
                    Report.Error($"{PositionOf(statement.Expressions[i])} Expected {output[i].Value} but found {type}.");
            }
        }

        private static string Analyze(Expression expression, string expectedType)
        {
            if (expression is Number number)
            {
                Analyze(number, expectedType);
                return number.Tag.ToString().ToLower();
            }
            else
                throw new NotImplementedException($"Cannot analyze {expression.GetType().Name}.");
        }

        private static void Analyze(Number expression, string expectedType)
        {
            if (expression.Tag == NumberTag.IntegerToBeInfered)
            {
                switch (expectedType)
                {
                    case "i8":
                        expression.Tag = NumberTag.I8;
                        break;
                    case "i16":
                        expression.Tag = NumberTag.I16;
                        break;
                    case "i32":
                        expression.Tag = NumberTag.I32;
                        break;
                    case "i64":
                        expression.Tag = NumberTag.I64;
                        break;
                    case "u8":
                        expression.Tag = NumberTag.U8;
                        break;
                    case "u16":
                        expression.Tag = NumberTag.U16;
                        break;
                    case "u32":
                        expression.Tag = NumberTag.U32;
                        break;
                    case "u64":
                        expression.Tag = NumberTag.U64;
                        break;
                    default:
                        throw new NotImplementedException($"Cannot infer {expectedType}.");
                }
            }
        }

        private static Position PositionOf(Expression expression)
        {
            if (expression is Number number)
                return number.DeclaredAt;
            else
                throw new NotImplementedException($"Cannot find position of {expression.GetType().Name}.");
        }
    }
}