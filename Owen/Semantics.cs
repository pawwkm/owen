using System;
using System.Collections.Generic;
using System.Linq;

namespace Owen
{
    internal static class Semantics
    {
        public static void Analyze(Program program)
        {
            program.Scope.Symbols.Add("i8", new PrimitiveType() { Tag = PrimitiveTypeTag.I8 });
            program.Scope.Symbols.Add("i16", new PrimitiveType() { Tag = PrimitiveTypeTag.I16 });
            program.Scope.Symbols.Add("i32", new PrimitiveType() { Tag = PrimitiveTypeTag.I32 });
            program.Scope.Symbols.Add("i64", new PrimitiveType() { Tag = PrimitiveTypeTag.I64 });
            program.Scope.Symbols.Add("u8", new PrimitiveType() { Tag = PrimitiveTypeTag.U8 });
            program.Scope.Symbols.Add("u16", new PrimitiveType() { Tag = PrimitiveTypeTag.U16 });
            program.Scope.Symbols.Add("u32", new PrimitiveType() { Tag = PrimitiveTypeTag.U32 });
            program.Scope.Symbols.Add("u64", new PrimitiveType() { Tag = PrimitiveTypeTag.U64 });
            program.Scope.Symbols.Add("f32", new PrimitiveType() { Tag = PrimitiveTypeTag.F32 });
            program.Scope.Symbols.Add("f64", new PrimitiveType() { Tag = PrimitiveTypeTag.F64 });

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
            foreach (var function in file.Functions)
                Analyze(function, program.Scope);
        }

        private static void Analyze(FunctionDeclaration function, Scope parent)
        {
            Analyze(function.Body, function.Output, parent);
        }

        private static void Analyze(CompoundStatement compound, List<Identifier> output, Scope parent)
        {
            compound.Scope = new Scope();
            compound.Scope.Parent = parent;

            foreach (var statement in compound.Statements)
            {
                if (statement is ReturnStatement r)
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

        private static Type Analyze(Expression expression, Type expectedType, Scope scope)
        {
            if (expression is Number number)
            {
                if (number.Tag == NumberTag.IntegerToBeInfered)
                    number.Tag = (NumberTag)(((PrimitiveType)expectedType).Tag);

                return Lookup(scope, number.Tag.ToString().ToLower());
            }
            else
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
                if (symbol.Key == name)
                    return symbol.Value;
            }

            if (scope.Parent == null)
                return null;
            else
                return Lookup(scope.Parent, name);
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