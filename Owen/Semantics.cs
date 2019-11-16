using System;
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
            var fileScope = new Scope();
            fileScope.Parent = program.Scope;

            foreach (var function in file.Functions)
            {
                var type = new FunctionType();
                type.Declaration = function;
                type.Input.AddRange(function.Input.Select(o => Lookup(fileScope, o.Type)));
                type.Output.AddRange(function.Output.Select(o => Lookup(fileScope, o.Value)));

                fileScope.Symbols.Add(new Symbol()
                {
                    Name = function.Name.Value, 
                    Type = type
                });
            }

            foreach (var function in file.Functions)
            {
                function.Body.Scope.Parent = fileScope;
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

                Analyze(function);
            }
        }

        private static void Analyze(FunctionDeclaration function)
        {
            Analyze(function.Body, function.Output, function.Body.Scope);
        }

        private static void Analyze(CompoundStatement compound, List<Identifier> output, Scope parent)
        {
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
                var expression = statement.Expressions[i];
                var outputType = Lookup(scope, output[i]);
                var expressionType = Analyze(ref expression, outputType, scope);

                if (expressionType != outputType)
                    Report.Error($"{PositionOf(statement.Expressions[i])} Expected {outputType} but found {expressionType}.");
                else
                    statement.Expressions[i] = expression;
            }
        }

        private static Type Analyze(ref Expression expression, Type expectedType, Scope scope)
        {
            if (expression is Number number)
            {
                if (number.Tag == NumberTag.IntegerToBeInfered)
                    number.Tag = (NumberTag)(((PrimitiveType)expectedType).Tag);

                return Lookup(scope, number.Tag.ToString().ToLower());
            }
            else if (expression is Identifier reference)
                return Lookup(scope, reference);
            else if (expression is Call call)
            {
                if (Analyze(ref call.Callee, null, scope) is FunctionType type)
                {
                    if (call.Arguments.Count != type.Input.Count)
                        Report.Error($"{PositionOf(call.Callee)} No function overload fitting the given input.");

                    for (var i = 0; i < call.Arguments.Count; i++)
                    {
                        var argument = call.Arguments[i];
                        if (!Compare(type.Input[i], Analyze(ref argument, type.Input[i], scope)))
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
            else if (expression is Ctfe ctfe)
            {
                Analyze(ref ctfe.Expression, expectedType, scope);
                Interpreter.Run(ref ctfe.Expression);

                expression = ctfe.Expression;

                return Analyze(ref expression, expectedType, scope);
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
            else
                throw new NotImplementedException($"Cannot find position of {expression.GetType().Name}.");
        }
    }
}