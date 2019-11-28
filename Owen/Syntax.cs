using System.Collections.Generic;
using System.Linq;

namespace Owen
{
    internal static class Syntax
    {
		public static void Parse(Program program, Source source)
        {
            var file = new File();
            file.Path = source.Position.Path;

            Whitespace(source);

            Expect(source, "namespace");
            if ((file.Namespace = Identifier(source)) == null)
                Report.Error("Identifier expected.");

            while (!source.EndOfText)
            {
                var function = FunctionDeclaration(source);
                if (function != null)
                    file.Functions.Add(function);

                var expression = default(Expression);
                if (Consume(source, "ctfe"))
                {
                    expression = Expression(source);
                    if (expression == null)
                        Report.Error($"{source.Position} Expression expected.");
                    else
                        file.Ctfe.Add(expression);
                }

                if (function == null && expression == null)
                    Report.Error($"{source.Position} Function or CTFE expression expected.");
            }

            program.Files.Add(file);
        }

        private static FunctionDeclaration FunctionDeclaration(Source source)
        {
            if (Consume(source, "function"))
            {
                var name = Identifier(source);
                if (name == null)
                    Report.Error("Identifier expected.");

                var declaration = new FunctionDeclaration();
                declaration.Name = name;

                if (Consume(source, "input"))
                {
                    do
                    {
                        var argument = new Argument();
                        var identifier = default(Identifier);

                        if ((identifier = Identifier(source)) == null)
                            Report.Error("Type expected.");
                        else if ((argument.Name = Identifier(source)) == null)
                            Report.Error("Type expected.");
                        else
                        {
                            argument.Type = new UnresolvedType()
                            {
                                Identifier = identifier
                            };

                            declaration.Input.Add(argument);
                        }
                    } while (Consume(source, ","));
                }

                if (Consume(source, "output"))
                {
                    do
                    {
                        var identifier = Identifier(source);
                        if (identifier == null)
                            Report.Error("Identifier expected.");
                        else
                            declaration.Output.Add(new UnresolvedType()
                            {
                                Identifier = identifier
                            });
                    } while (Consume(source, ","));
                }

                declaration.Body = CompoundStatement(source);

                return declaration;
            }
            else
                return null;
        }

        private static CompoundStatement CompoundStatement(Source source)
        {
            var compound = new CompoundStatement();
            while (true)
            {
                var statement = AssignmentStatement(source) ??
                                ExpressionStatement(source) ??
                                ReturnStatement(source) ??
                                AssertStatement(source);
                
                if (statement == null)
                    break;
                else
                    compound.Statements.Add(statement);
            }

            Expect(source, "end");

            return compound;
        }

        private static Statement AssignmentStatement(Source source)
        {
            var start = source.Index;

            var left = Expressions(source);
            if (left.Count != 0)
            {
                var assignment = new AssignmentStatement();
                assignment.Left = left;
                assignment.Operator = new Operator();
                assignment.Operator.Start = source.Position.Copy();

                if (Consume(source, "+="))
                    assignment.Operator.Tag = OperatorTag.PlusEqual;
                else if (Consume(source, "-="))
                    assignment.Operator.Tag = OperatorTag.MinusEqual;
                else if (Consume(source, "*="))
                    assignment.Operator.Tag = OperatorTag.MultiplyEqual;
                else if (Consume(source, "/="))
                    assignment.Operator.Tag = OperatorTag.DivideEqual;
                else if (Consume(source, "&="))
                    assignment.Operator.Tag = OperatorTag.BitwiseAndEqual;
                else if (Consume(source, "|="))
                    assignment.Operator.Tag = OperatorTag.BitwiseOrEqual;
                else if (Consume(source, "^="))
                    assignment.Operator.Tag = OperatorTag.BitwiseXorEqual;
                else if (Consume(source, "%="))
                    assignment.Operator.Tag = OperatorTag.ModuloEqual;
                else if (Consume(source, "<<="))
                    assignment.Operator.Tag = OperatorTag.LeftShiftEqual;
                else if (Consume(source, ">>="))
                    assignment.Operator.Tag = OperatorTag.RightShiftEqual;
                else if (Consume(source, "="))
                    assignment.Operator.Tag = OperatorTag.Equal;
                else if (left.Count > 1)
                    Report.Error($"{source.Position} Assignment operator expected.");
                else
                {
                    source.Index = start;
                    return null;
                }

                assignment.Right = Expressions(source);
                if (assignment.Right.Count == 0)
                    Report.Error($"{source.Position} One or more expressions expected.");

                return assignment;
            }
            else
                return null;
        }

        private static Statement ExpressionStatement(Source source)
        {
            var expression = Expression(source);
            if (expression == null)
                return null;
            else
                return new ExpressionStatement()
                {
                    Expression = expression
                };
        }

        private static Statement ReturnStatement(Source source)
        {
            if (Consume(source, "return"))
            {
                UpdatePosition(source, source.Index);
                return new ReturnStatement()
                {
                    EndOfKeyword = source.Position.Copy(),
                    Expressions = Expressions(source)
                };
            }
            else
                return null;
        }

        private static Statement AssertStatement(Source source)
        {
            if (Consume(source, "assert"))
            {
                var assertion = Expression(source);
                if (assertion == null)
                    Report.Error($"{source.Position} Assertion expected.");
                else
                    return new AssertStatement()
                    {
                        Assertion = assertion
                    };
            }
            
            return null;
        }

        private static List<Expression> Expressions(Source source)
        {
            var expressions = new List<Expression>();

            var expression = Expression(source);
            if (expression != null)
            {
                expressions.Add(expression);
                while (Consume(source, ","))
                {
                    expression = Expression(source);
                    if (expression == null)
                        Report.Error("Expression expected.");
                    else
                        expressions.Add(expression);
                }
            }
            
            return expressions;
        }

        private static Expression Expression(Source source)
        {
            return AdditiveExpression(source);
        }

        private static Expression AdditiveExpression(Source source)
        {
            var left = PrefixExpression(source);
            while (true)
            {
                var start = source.Position.Copy();
                var tag = default(OperatorTag);

                if (Consume(source, "=="))
                    tag = OperatorTag.EqualEqual;
                else if (Consume(source, "!="))
                    tag = OperatorTag.NotEqual;
                else
                    break;

                var right = PrefixExpression(source);
                if (right == null)
                    Report.Error($"{source.Position} Expression expected.");
                else
                {
                    if (left is BinaryExpression binary)
                        binary.Right.End = right.End;

                    left = new BinaryExpression()
                    {
                        Start = left.Start,
                        Left = left,
                        Operator = new Operator()
                        {
                            Start = start,
                            Tag = tag
                        },
                        Right = right
                    };
                }
            }

            return left;
        }

        private static Expression PrefixExpression(Source source)
        {
            return PostfixExpression(source);
        }

        private static Expression PostfixExpression(Source source)
        {
            var expression = PrimaryExpression(source);
            if (expression != null)
            {
                if (Consume(source, "("))
                {
                    var arguments = Expressions(source);
                    Expect(source, ")");

                    return new Call()
                    {
                        Start = expression.Start,
                        Reference = expression,
                        Arguments = arguments,
                        End = arguments.Count == 0 ? expression.End :
                                                     arguments.Last().End
                    };
                }
                else
                    return expression;
            }
            else
                return null;
        }

        private static Expression PrimaryExpression(Source source)
        {
            return Number(source) ??
                   Identifier(source);
        }

        private static Expression Number(Source source)
        {
            var start = source.Index;
            if (source.Text[source.Index] == '-')
                source.Index++;

            while (!source.EndOfText && IsDigit(source.Text[source.Index]))
                source.Index++;

            if (source.Index == start)
                return null;
            else if (source.Index - start == 1 && source.Text[start] == '-')
            {
                UpdatePosition(source, source.Index);
                Report.Error($"{source.Position} Digit expected.");
            }

            if (source.Text[source.Index] == '.')
            {
                var afterDot = ++source.Index;
                while (!source.EndOfText && IsDigit(source.Text[source.Index]))
                    source.Index++;

                if (source.Index == afterDot)
                {
                    UpdatePosition(source, source.Index);
                    Report.Error($"{source.Position} Digit expected.");
                }

                var number = new Number();
                number.Value = source.Text.Substring(start, source.Index - start);

                UpdatePosition(source, start);
                number.Start = source.Position.Copy();
                number.End = source.Position.Copy();
                number.End.Column += number.Value.Length;

                if (Consume(source, "f32"))
                    number.Tag = NumberTag.F32;
                else if (Consume(source, "f64"))
                    number.Tag = NumberTag.F64;
                else
                {
                    number.Tag = NumberTag.ToBeInfered;
                    Whitespace(source);
                }

                return number;
            }
            else
            {
                var number = new Number();
                number.Value = source.Text.Substring(start, source.Index - start);

                UpdatePosition(source, start);
                number.Start = source.Position.Copy();
                number.End = source.Position.Copy();
                number.End.Column += number.Value.Length;

                if (Consume(source, "i8"))
                    number.Tag = NumberTag.I8;
                else if (Consume(source, "i16"))
                    number.Tag = NumberTag.I16;
                else if (Consume(source, "i32"))
                    number.Tag = NumberTag.I32;
                else if (Consume(source, "i64"))
                    number.Tag = NumberTag.I64;
                else if (Consume(source, "u8"))
                    number.Tag = NumberTag.U8;
                else if (Consume(source, "u16"))
                    number.Tag = NumberTag.U16;
                else if (Consume(source, "u32"))
                    number.Tag = NumberTag.U32;
                else if (Consume(source, "u64"))
                    number.Tag = NumberTag.U64;
                else
                {
                    number.Tag = NumberTag.ToBeInfered;
                    Whitespace(source);
                }

                return number;
            }
        }

        private static Identifier Identifier(Source source)
        {
            var start = source.Index;
            if (!source.EndOfText && IsLetter(source.Text[source.Index]))
            {
                while (!source.EndOfText && (IsLetterOrDigit(source.Text[source.Index]) || source.Text[source.Index] == '_'))
                    source.Index++;
            }

            if (source.Index == start)
                return null;
            else
            {
                var identifier = new Identifier();
                identifier.Value = source.Text.Substring(start, source.Index - start);

                var keywords = new[]
                { 
                    "namespace", "use", "public", "function", "input", "output", "end", 
                    "if", "else", "for", "while", "break", "structure", "proposition", 
                    "enumeration", "of", "size", "union", "return", "mixin", "ctfe", 
                    "true", "false", "assert", "null", "generalize", "version" 
                };

                if (keywords.Contains(identifier.Value))
                {
                    source.Index = start;
                    return null;
                }
                else
                {
                    UpdatePosition(source, start);
                    identifier.Start = source.Position.Copy();
                    identifier.End = source.Position.Copy();
                    identifier.End.Column += identifier.Value.Length;

                    Whitespace(source);

                    return identifier;
                }
            }
        }

        private static bool IsLetterOrDigit(char c)
        {
            return IsLetter(c) || IsDigit(c);
        }

        private static bool IsLetter(char c)
        {
            return c >= 'A' && c <= 'Z' ||
                   c >= 'a' && c <= 'z';
        }

        private static bool IsDigit(char c)
        {
            return c >= '0' && c <= '9';
        }

        private static void Whitespace(Source source)
        {
            while (!source.EndOfText)
            {
                switch (source.Text[source.Index])
                {
                    case '\n':
                    case '\r':
                    case ' ':
                        source.Index++;
                        break;
                    case '/':
                        if (source.Index + 1 <= source.Text.Length && source.Text[source.Index + 1] == '/')
                        {
                            while (!source.EndOfText && source.Text[source.Index] != '\n')
                                source.Index++;

                            break;
                        }
                        else
                            return;
                    default:
                        return;
                }
            }
        }

        private static bool Consume(Source source, string value)
        {
            if (source.Index + value.Length <= source.Text.Length)
            {
                for (var i = 0; i < value.Length; i++)
                {
                    if (source.Text[source.Index + i] != value[i])
                        return false;
                }

                source.Index += value.Length;
                Whitespace(source);

                return true;
            }
            else
                return false;
        }

        private static void Expect(Source source, string value)
        {
            if (!Consume(source, value))
            {
                UpdatePosition(source, source.Index);
                Report.Error($"{source.Position} {value} expected.");
            }
        }

        private static void UpdatePosition(Source source, int index)
        {
            for (var i = source.Position.Index; i < index; i++)
            {
                switch (source.Text[i])
                {
                    case '\n':
                        source.Position.Line++;
                        source.Position.Column = 1;
                        break;
                    default:
                        source.Position.Column++;
                        break;
                }
            }

            source.Position.Index = index;
        }
    }
}