using System.Collections.Generic;
using System.Linq;

namespace Owen
{
    internal static class Syntax
    {
        private static List<Token> Tokenize(string text, string path)
        {
            var position = new Position();
            position.Path = path;

            var tokens = new List<Token>();
            var index = 0;
            var startIndex = 0;
            var startPostion = default(Position);

            bool Match(string value)
            {
                if (index + value.Length <= text.Length)
                {
                    for (var i = 0; i < value.Length; i++)
                    {
                        if (text[index + i] != value[i])
                            return false;
                    }

                    return true;
                }
                else
                    return false;
            }

            bool Skip(string value)
            {
                if (Match(value))
                {
                    position.Column += value.Length;
                    index += value.Length;

                    return true;
                }
                else
                    return false;
            }

            bool Consume(string value, TokenTag tag)
            {
                if (Skip(value))
                {
                    tokens.Add(new Token()
                    {
                        Start = startPostion,
                        End = position.Copy(),
                        Value = value,
                        Tag = tag
                    });

                    return true;
                }
                else
                    return false;
            }

            while (text.Length != index)
            {
                startIndex = index;
                startPostion = position.Copy();

                if (text[index] == ' ' || text[index] == '\r')
                {
                    index++;
                    position.Column++;
                }
                else if (text[index] == '\n')
                {
                    index++;
                    position.Line++;
                    position.Column = 1;
                }
                else if (Match("//"))
                {
                    while (text.Length != index && text[index] != '\n')
                    {
                        index++;
                        position.Column++;
                    }
                }
                else if (Consume("+=", TokenTag.PlusEqual) ||
                         Consume("-=", TokenTag.MinusEqual) ||
                         Consume("*=", TokenTag.MultiplyEqual) ||
                         Consume("/=", TokenTag.DivideEqual) ||
                         Consume("&=", TokenTag.BitwiseAndEqual) ||
                         Consume("|=", TokenTag.BitwiseOrEqual) ||
                         Consume("^=", TokenTag.BitwiseXorEqual) ||
                         Consume("%=", TokenTag.ModuloEqual) ||
                         Consume("<<=", TokenTag.LeftShiftEqual) ||
                         Consume(">>=", TokenTag.RightShiftEqual) ||
                         Consume("||", TokenTag.LogicalOr) ||
                         Consume("&&", TokenTag.LogicalAnd) ||
                         Consume("==", TokenTag.EqualEqual) ||
                         Consume("=", TokenTag.Equal) ||
                         Consume("!=", TokenTag.NotEqual) ||
                         Consume("<=", TokenTag.LessThanOrEqual) ||
                         Consume(">=", TokenTag.GreaterThanOrEqual) ||
                         Consume("+", TokenTag.Plus) ||
                         Consume("-", TokenTag.Minus) ||
                         Consume("|", TokenTag.BitwiseOr) ||
                         Consume("^", TokenTag.BitwiseXor) ||
                         Consume("*", TokenTag.Multiply) ||
                         Consume("/", TokenTag.Divide) ||
                         Consume("%", TokenTag.Modulo) ||
                         Consume("&", TokenTag.BitwiseAnd) ||
                         Consume("<<", TokenTag.LeftShift) ||
                         Consume(">>", TokenTag.RightShift) ||
                         Consume("<", TokenTag.LessThan) ||
                         Consume(">", TokenTag.GreaterThan) ||
                         Consume("!", TokenTag.Not) ||
                         Consume("#", TokenTag.AddressOf) ||
                         Consume("@", TokenTag.Dereference) ||
                         Consume(".", TokenTag.Dot) ||
                         Consume("(", TokenTag.LeftParentheses) ||
                         Consume(")", TokenTag.RightParentheses) ||
                         Consume("[", TokenTag.LeftSquareBracket) ||
                         Consume("]", TokenTag.RightSquareBracket) ||
                         Consume(",", TokenTag.Comma) ||
                         Consume(";", TokenTag.Semicolon) ||
                         Consume(":", TokenTag.Colon))
                    ;
                else if (text[index] >= 'A' && text[index] <= 'Z' || text[index] >= 'a' && text[index] <= 'z')
                {
                    while (text.Length != index &&
                          (text[index] >= 'A' && text[index] <= 'Z' ||
                           text[index] >= 'a' && text[index] <= 'z' ||
                           text[index] >= '0' && text[index] <= '9' ||
                           text[index] == '_'))
                    {
                        index++;
                        position.Column++;
                    }

                    var tag = TokenTag.Identifier;
                    var keywordOrIdentifier = text.Substring(startIndex, index - startIndex);

                    switch (keywordOrIdentifier)
                    {
                        case "namespace":
                            tag = TokenTag.Namespace;
                            break;
                        case "use":
                            tag = TokenTag.Use;
                            break;
                        case "public":
                            tag = TokenTag.Public;
                            break;
                        case "external":
                            tag = TokenTag.External;
                            break;
                        case "function":
                            tag = TokenTag.Function;
                            break;
                        case "input":
                            tag = TokenTag.Input;
                            break;
                        case "output":
                            tag = TokenTag.Output;
                            break;
                        case "end":
                            tag = TokenTag.End;
                            break;
                        case "if":
                            tag = TokenTag.If;
                            break;
                        case "else":
                            tag = TokenTag.Else;
                            break;
                        case "for":
                            tag = TokenTag.For;
                            break;
                        case "while":
                            tag = TokenTag.While;
                            break;
                        case "break":
                            tag = TokenTag.Break;
                            break;
                        case "structure":
                            tag = TokenTag.Structure;
                            break;
                        case "proposition":
                            tag = TokenTag.Proposition;
                            break;
                        case "enumeration":
                            tag = TokenTag.Enumeration;
                            break;
                        case "of":
                            tag = TokenTag.Of;
                            break;
                        case "size":
                            tag = TokenTag.Size;
                            break;
                        case "union":
                            tag = TokenTag.Union;
                            break;
                        case "return":
                            tag = TokenTag.Return;
                            break;
                        case "mixin":
                            tag = TokenTag.Mixin;
                            break;
                        case "ctfe":
                            tag = TokenTag.Ctfe;
                            break;
                        case "true":
                            tag = TokenTag.True;
                            break;
                        case "false":
                            tag = TokenTag.False;
                            break;
                        case "assert":
                            tag = TokenTag.Assert;
                            break;
                        case "null":
                            tag = TokenTag.Null;
                            break;
                        case "generalize":
                            tag = TokenTag.Generalize;
                            break;
                        case "version":
                            tag = TokenTag.Version;
                            break;
                        case "cast":
                            tag = TokenTag.Cast;
                            break;
                    }

                    tokens.Add(new Token()
                    {
                        Start = startPostion,
                        Value = keywordOrIdentifier,
                        End = position.Copy(),
                        Tag = tag
                    });
                }
                else if (text.Length != index && text[index] >= '0' && text[index] <= '9')
                {
                    while (text.Length != index && text[index] >= '0' && text[index] <= '9')
                    {
                        index++;
                        position.Column++;
                    }

                    if (text.Length != index && text[index] == '.')
                    {
                        index++;
                        position.Column++;

                        if (text.Length != index && text[index] >= '0' && text[index] <= '9')
                        {
                            while (text.Length != index && text[index] >= '0' && text[index] <= '9')
                            {
                                index++;
                                position.Column++;
                            }

                            var startOfType = index;
                            var endOfNumber = position.Copy();

                            var tag = TokenTag.NumberToBeInfered;
                            if (Skip("f32"))
                                tag = TokenTag.F32;
                            else if (Skip("f64"))
                                tag = TokenTag.F64;

                            tokens.Add(new Token()
                            {
                                Start = startPostion,
                                Value = text.Substring(startIndex, startOfType - startIndex),
                                Tag = tag,
                                End = endOfNumber
                            });
                        }
                        else
                            Report.Error($"{position} Digit expected.");
                    }
                    else
                    {
                        var startOfType = index;
                        var endOfNumber = position.Copy();

                        var tag = TokenTag.NumberToBeInfered;
                        if (Skip("i8"))
                            tag = TokenTag.I8;
                        else if (Skip("i16"))
                            tag = TokenTag.I16;
                        else if (Skip("i32"))
                            tag = TokenTag.I32;
                        else if (Skip("i64"))
                            tag = TokenTag.I64;
                        else if (Skip("u8"))
                            tag = TokenTag.U8;
                        else if (Skip("u16"))
                            tag = TokenTag.U16;
                        else if (Skip("u32"))
                            tag = TokenTag.U32;
                        else if (Skip("u64"))
                            tag = TokenTag.U64;

                        tokens.Add(new Token()
                        {
                            Start = startPostion,
                            Value = text.Substring(startIndex, startOfType - startIndex),
                            Tag = tag,
                            End = endOfNumber
                        });
                    }
                }
                else if (text[index] == '"')
                {
                    index++;
                    position.Column++;

                    while (text.Length != index && text[index] != '"')
                    {
                        if (text[index] == '\n')
                        {
                            position.Line++;
                            position.Column = 1;
                        }
                        else
                            position.Column++;

                        index++;
                    }

                    if (text[index] == '"')
                    {
                        index++;
                        position.Column++;
                    }
                    else
                        Report.Error($"{position} \" expected.");

                    tokens.Add(new Token()
                    {
                        Start = startPostion,
                        End = position.Copy(),
                        Value = text.Substring(startIndex + 1, index - startIndex - 1),
                        Tag = TokenTag.String
                    });
                }
                else
                    Report.Error($"{position} Unexpected character '{text[index]}'.");
            }

            return tokens;
        }

        public static File Parse(string text, string path)
        {
            var file = new File();
            file.Path = path;

            var source = new Source();
            source.Tokens = Tokenize(text, path);

            Expect(source, TokenTag.Namespace, "namespace");
            
            file.Namespace = Identifier(source);
            if (file.Namespace == null)
                Report.Error($"{source.Position} Identifier expected.");

            while (Consume(source, TokenTag.Use))
            {
                var use = Identifier(source);
                if (use == null)
                    Report.Error($"{source.Position} Identifier expected.");

                file.Uses.Add(use);
            }

            while (!source.EndOfTokens)
            {
                var isPublic = Consume(source, TokenTag.Public);

                var function = FunctionDeclaration(source, isPublic);
                if (function != null)
                    file.Functions.Add(function);
                else
                {
                    var proposition = PropositionDeclaration(source);
                    if (proposition != null)
                        file.Propositions.Add(proposition);
                    else
                    {
                        var compound = CompoundDeclaration(source, isPublic);
                        if (compound != null)
                            file.Compounds.Add(compound);
                        else
                        {
                            var enumeration = EnumerationDeclaration(source, isPublic);
                            if (enumeration != null)
                                file.Enumerations.Add(enumeration);
                            else
                            {
                                if (Consume(source, TokenTag.Ctfe))
                                {
                                    var expression = Expression(source);
                                    if (expression == null)
                                        Report.Error($"{source.Position} Expression expected.");
                                    else
                                        file.Ctfe.Add(expression);
                                }
                                else
                                    Report.Error($"{source.Position} Function, proposition, structure, union, enumeration or CTFE expression expected.");
                            }
                        }
                    }
                }
            }

            return file;
        }

        private static FunctionDeclaration FunctionDeclaration(Source source, bool isPublic)
        {
            var isExternal = Consume(source, TokenTag.External);
            if (Consume(source, TokenTag.Function))
            {
                var name = Identifier(source);
                if (name == null)
                    Report.Error($"{source.Position} Identifier expected.");

                var declaration = new FunctionDeclaration();
                declaration.IsPublic = isPublic;
                declaration.Name = name;

                if (Consume(source, TokenTag.Generalize))
                {
                    do
                    {
                        var identifier = Identifier(source);
                        if (identifier == null)
                            Report.Error($"{source.Position} Identifier expected.");
                        else
                            declaration.Generalized.Add(identifier);
                    } while (Consume(source, TokenTag.Comma));
                }

                if (Consume(source, TokenTag.Input))
                {
                    do
                    {
                        var argument = new Argument();
                        argument.Type = Type(source);
                        if (argument.Type == null)
                            Report.Error($"{source.Position} Type expected.");

                        argument.Name = Identifier(source);
                        if (argument.Name == null)
                            Report.Error($"{source.Position} Identifier expected.");

                        declaration.Input.Add(argument);
                    } while (Consume(source, TokenTag.Comma));
                }

                if (Consume(source, TokenTag.Output))
                {
                    var types = new List<Type>();
                    do
                    {
                        var type = Type(source);
                        if (type == null)
                            Report.Error("Type expected.");
                        else
                            types.Add(type);
                    } while (Consume(source, TokenTag.Comma));

                    if (types.Count == 1)
                        declaration.Output = types[0];
                    else if (types.Count > 1)
                        declaration.Output = new TupleType()
                        {
                            Types = types
                        };
                }

                if (isExternal)
                {
                    declaration.Library = String(source);
                    if (declaration.Library == null)
                        Report.Error($"{source.Position} String expected.");

                    declaration.IsExternal = true;
                    declaration.Body = new CompoundStatement();
                }
                else
                {
                    declaration.Body = CompoundStatement(source);
                    Expect(source, TokenTag.End, "end");
                }

                return declaration;
            }
            else if (isExternal)
                Report.Error($"{source.Position} Function declaration expected.");
            
            return null;
        }

        private static CompoundStatement PropositionDeclaration(Source source)
        {
            if (Consume(source, TokenTag.Proposition))
            {
                var body = CompoundStatement(source);
                Expect(source, TokenTag.End, "end");

                return body;
            }
            else
                return null;
        }

        private static CompoundDeclaration CompoundDeclaration(Source source, bool isPublic)
        {
            CompoundTypeTag tag;
            if (Consume(source, TokenTag.Structure))
                tag = CompoundTypeTag.Structure;
            else if (Consume(source, TokenTag.Union))
                tag = CompoundTypeTag.Union;
            else
                return null;

            var declaration = new CompoundDeclaration()
            {
                IsPublic = isPublic,
                Tag = tag,
                Name = Identifier(source)
            };

            if (declaration.Name == null)
                Report.Error($"{source.Position} Identifier expected.");

            while (true)
            {
                var field = new Field();
                field.Type = Type(source);
                if (field.Type == null)
                    break;

                field.Name = Identifier(source);
                if (field.Name == null)
                    Report.Error($"{source.Position} Identifier expected.");

                declaration.Fields.Add(field);
            }

            Expect(source, TokenTag.End, "end");

            return declaration;
        }

        private static EnumerationDeclaration EnumerationDeclaration(Source source, bool isPublic)
        {
            if (Consume(source, TokenTag.Enumeration))
            {
                var declaration = new EnumerationDeclaration();
                declaration.IsPublic = isPublic;
                declaration.Name = Identifier(source);
                if (declaration.Name == null)
                    Report.Error($"{source.Position} Identifier expected.");

                Expect(source, TokenTag.Of, "of");
                
                declaration.Type = Type(source);
                if (declaration.Type == null)
                    Report.Error($"{source.Position} Type expected.");

                while (true)
                {
                    var name = Identifier(source);
                    if (name == null)
                        break;
                    else
                    {
                        var constant = new EnumerationConstant();
                        constant.Name = name;

                        if (Consume(source, TokenTag.Equal))
                        {
                            var value = Number(source);
                            if (value == null || value.Value.Contains("."))
                                Report.Error($"{source.Position} Integer expected.");
                            else
                                constant.Value = value;
                        }

                        declaration.Constants.Add(constant);
                    }
                }

                Expect(source, TokenTag.End, "end");

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
                                IfStatement(source) ??
                                ForStatement(source) ??
                                WhileStatement(source) ??
                                BreakStatement(source) ??
                                ReturnStatement(source) ??
                                AssertStatement(source);
                
                if (statement == null)
                    break;
                else
                    compound.Statements.Add(statement);
            }

            return compound;
        }

        private static Statement AssignmentStatement(Source source)
        {
            var start = source.Index;

            var left = Expressions(source);
            if (left.Count != 0)
            {
                if (source.Current?.Tag >= TokenTag.PlusEqual && source.Current?.Tag <= TokenTag.Equal)
                {
                    var assignment = new AssignmentStatement();
                    assignment.Left = left;
                    assignment.Operator = Operator(source);

                    assignment.Right = Expressions(source);
                    if (assignment.Right.Count == 0)
                        Report.Error($"{source.Position} One or more expressions expected.");

                    return assignment;
                }
                else if (left.Count > 1)
                    Report.Error($"{source.Position} Assignment operator expected.");
                else
                    source.Index = start;
            }
            
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

        private static Statement IfStatement(Source source)
        {
            if (Consume(source, TokenTag.If))
            {
                var block = new ConditionalBlock();
                
                block.Assignment = (AssignmentStatement)AssignmentStatement(source);
                if (block.Assignment != null)
                    Expect(source, TokenTag.Semicolon, ";");

                block.Condition = Expression(source);
                if (block.Condition == null)
                    Report.Error($"{source.Position} Expression expected.");

                block.Body = CompoundStatement(source);

                var statement = new IfStatement();
                statement.Blocks.Add(block);

                while (Consume(source, TokenTag.Else))
                {
                    block = new ConditionalBlock();
                    if (Consume(source, TokenTag.If))
                    {
                        block.Assignment = (AssignmentStatement)AssignmentStatement(source);
                        if (block.Assignment != null)
                            Expect(source, TokenTag.Semicolon, ";");

                        block.Condition = Expression(source);
                        if (block.Condition == null)
                            Report.Error($"{source.Position} Expression expected.");
                    }

                    block.Body = CompoundStatement(source);
                    statement.Blocks.Add(block);

                    if (block.Condition == null)
                        break;
                }

                Expect(source, TokenTag.End, "end");

                return statement;
            }
            else
                return null;
        }

        private static Statement ForStatement(Source source)
        {
            if (Consume(source, TokenTag.For))
            {
                var statement = new ForStatement();
                statement.Assignment = (AssignmentStatement)AssignmentStatement(source);
                if (statement.Assignment == null)
                    Report.Error($"{source.Position} Assignment expected.");

                Expect(source, TokenTag.Semicolon, ";");

                statement.Condition = Expression(source);
                if (statement.Condition == null)
                    Report.Error($"{source.Position} Expression expected.");

                Expect(source, TokenTag.Semicolon, ";");
                statement.Post = (AssignmentStatement)AssignmentStatement(source);

                statement.Body = CompoundStatement(source);
                Expect(source, TokenTag.End, "end");

                return statement;
            }
            else
                return null;
        }

        private static Statement WhileStatement(Source source)
        {
            if (Consume(source, TokenTag.While))
            {
                var statement = new WhileStatement();
                statement.Assignment = (AssignmentStatement)AssignmentStatement(source);

                if (statement.Assignment != null)
                    Expect(source, TokenTag.Semicolon, ";");

                statement.Condition = Expression(source);
                if (statement.Condition == null)
                    Report.Error($"{source.Position} Expression expected.");

                statement.Body = CompoundStatement(source);
                Expect(source, TokenTag.End, "end");

                return statement;
            }
            else
                return null;
        }

        private static Statement BreakStatement(Source source)
        {
            if (Consume(source, TokenTag.Break))
            {
                return new BreakStatement()
                {
                    Start = source.Tokens[source.Index - 1].Start,
                };
            }
            else
                return null;
        }

        private static Statement ReturnStatement(Source source)
        {
            if (Consume(source, TokenTag.Return))
            {
                return new ReturnStatement()
                {
                    Start = source.Tokens[source.Index - 1].Start,
                    Expressions = Expressions(source)
                };
            }
            else
                return null;
        }

        private static Statement AssertStatement(Source source)
        {
            if (Consume(source, TokenTag.Assert))
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
                while (Consume(source, TokenTag.Comma))
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
            return LogicalOrExpression(source);
        }

        private static Expression LogicalOrExpression(Source source)
        {
            var left = LogicalAndExpression(source);
            while (left != null)
            {
                if (source.Current?.Tag == TokenTag.LogicalOr)
                {
                    var op = Operator(source);
                    var right = LogicalAndExpression(source);
                    if (right == null)
                        Report.Error($"{source.Position} Expression expected.");
                    else
                    {
                        left = new BinaryExpression()
                        {
                            Start = left.Start,
                            Left = left,
                            Operator = op,
                            Right = right
                        };
                    }
                }
                else
                    break;
            }

            return left;
        }

        private static Expression LogicalAndExpression(Source source)
        {
            var left = RelationalExpression(source);
            while (left != null)
            {
                if (source.Current?.Tag == TokenTag.LogicalAnd)
                {
                    var op = Operator(source);
                    var right = RelationalExpression(source);
                    if (right == null)
                        Report.Error($"{source.Position} Expression expected.");
                    else
                    {
                        left = new BinaryExpression()
                        {
                            Start = left.Start,
                            Left = left,
                            Operator = op,
                            Right = right
                        };
                    }
                }
                else
                    break;
            }

            return left;
        }

        private static Expression RelationalExpression(Source source)
        {
            var left = AdditiveExpression(source);
            while (left != null)
            {
                if (source.Current?.Tag >= TokenTag.EqualEqual && source.Current?.Tag <= TokenTag.GreaterThan)
                {
                    var op = Operator(source);
                    var right = AdditiveExpression(source);
                    if (right == null)
                        Report.Error($"{source.Position} Expression expected.");
                    else
                    {
                        left = new BinaryExpression()
                        {
                            Start = left.Start,
                            Left = left,
                            Operator = op,
                            Right = right
                        };
                    }
                }
                else
                    break;
            }

            return left;
        }

        private static Expression AdditiveExpression(Source source)
        {
            var left = MultiplicativeExpression(source);
            while (left != null)
            {
                if (source.Current?.Tag >= TokenTag.Plus && source.Current?.Tag <= TokenTag.BitwiseXor)
                {
                    var op = Operator(source);
                    var right = MultiplicativeExpression(source);
                    if (right == null)
                        Report.Error($"{source.Position} Expression expected.");
                    else
                    {
                        left = new BinaryExpression()
                        {
                            Start = left.Start,
                            Left = left,
                            Operator = op,
                            Right = right
                        };
                    }
                }
                else
                    break;
            }

            return left;
        }

        private static Expression MultiplicativeExpression(Source source)
        {
            var left = PrefixExpression(source);
            while (left != null)
            {
                if (source.Current?.Tag >= TokenTag.Multiply && source.Current?.Tag <= TokenTag.RightShift)
                {
                    var op = Operator(source);
                    var right = PrefixExpression(source);
                    if (right == null)
                        Report.Error($"{source.Position} Expression expected.");
                    else
                    {
                        left = new BinaryExpression()
                        {
                            Start = left.Start,
                            Left = left,
                            Operator = op,
                            Right = right
                        };
                    }
                }
                else
                    break;
            }

            return left;
        }

        private static Expression PrefixExpression(Source source)
        {
            var start = source.Position;
            if (Consume(source, TokenTag.Not))
            {
                var expression = PostfixExpression(source);
                if (expression == null)
                    Report.Error($"{source.Position} Expression expected.");

                return new Not()
                {
                    Start = start,
                    Expression = expression
                };
            }
            else if (Consume(source, TokenTag.Minus))
            {
                var expression = PostfixExpression(source);
                if (expression == null)
                    Report.Error($"{source.Position} Expression expected.");

                return new Negate()
                {
                    Start = start,
                    Expression = expression
                };
            }
            else if (Consume(source, TokenTag.Size))
            {
                Expect(source, TokenTag.Of, "of");

                var type = Identifier(source);
                if (type == null)
                    Report.Error($"{source.Position} Identifier expected.");

                return new SizeOf()
                {
                    Start = start,
                    TypeBeingSizedUp = new UnresolvedType()
                    {
                        Identifier = type
                    }
                };
            }
            else if (Consume(source, TokenTag.AddressOf))
            {
                var expression = PostfixExpression(source);
                if (expression == null)
                    Report.Error($"{source.Position} Expression expected.");

                return new AddressOf()
                {
                    Start = start,
                    Expression = expression
                };
            }
            else if (Consume(source, TokenTag.Dereference))
            {
                var expression = PostfixExpression(source);
                if (expression == null)
                    Report.Error($"{source.Position} Expression expected.");

                return new Dereference()
                {
                    Start = start,
                    Expression = expression
                };
            }
            else if (Consume(source, TokenTag.Cast))
            {
                Expect(source, TokenTag.LeftParentheses, "(");
                var type = Type(source);
                if (type == null)
                    Report.Error($"{source.Position} Type expected.");

                Expect(source, TokenTag.RightParentheses, ")");
                return new Cast()
                {
                    Start = start,
                    Expression = Expression(source),
                    To = type
                };
            }
            else
                return PostfixExpression(source);
        }

        private static Expression PostfixExpression(Source source)
        {
            var startOfInput = default(Position);
            var expression = PrimaryExpression(source);
            if (expression != null)
            {
                var start = source.Index;
                if (Consume(source, TokenTag.LessThan))
                {
                    var type = Type(source);
                    if (type == null)
                        source.Index = start;
                    else
                    {
                        var generics = new List<Type>();
                        generics.Add(type);

                        while (Consume(source, TokenTag.Comma))
                        {
                            type = Type(source);
                            if (type == null)
                                Report.Error($"{source.Position} Type expected.");

                            generics.Add(type);
                        }

                        if (Consume(source, TokenTag.GreaterThan))
                        {
                            startOfInput = source.Position;
                            Expect(source, TokenTag.LeftParentheses, "(");
                            var arguments = Expressions(source);
                            Expect(source, TokenTag.RightParentheses, ")");

                            return new Call()
                            {
                                Generics = generics,
                                Start = startOfInput,
                                Reference = expression,
                                Arguments = arguments
                            };
                        }
                        else
                            source.Index = start;
                    }
                }

                startOfInput = source.Position;
                if (Consume(source, TokenTag.LeftParentheses))
                {
                    var arguments = Expressions(source);
                    Expect(source, TokenTag.RightParentheses, ")");

                    return new Call()
                    {
                        Generics = new List<Type>(),
                        Start = startOfInput,
                        Reference = expression,
                        Arguments = arguments
                    };
                }
                else if (Consume(source, TokenTag.Dot))
                {
                    var field = PostfixExpression(source);
                    if (field == null)
                        Report.Error($"{source.Position} Expression expected.");

                    return new DotExpression()
                    {
                        Start = expression.Start,
                        Structure = expression,
                        Field = field
                    };
                }
                else if (Consume(source, TokenTag.LeftSquareBracket))
                {
                    var position = Expression(source);
                    if (position == null)
                        Report.Error($"{source.Position} Expression expected.");

                    Expect(source, TokenTag.RightSquareBracket, "]");

                    return new Index()
                    {
                        Start = startOfInput,
                        Array = expression,
                        Position = position
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
                   Boolean(source) ??
                   String(source) ??
                   NullLiteral(source) ??
                   CompoundLiteral(source) ??
                   ArrayLiteral(source) ??
                   Identifier(source) ??
                   ParentherizedExpression(source);
        }

        private static Expression CompoundLiteral(Source source)
        {
            var start = source.Index;
            var literal = new CompoundLiteral();
            literal.Structure = Identifier(source);

            if (literal.Structure == null)
                return null;
            else
                literal.Start = literal.Structure.Start;

            var initializer = new FieldInitializer();
            initializer.Name = Identifier(source);
            if (initializer.Name == null)
            {
                source.Index = start;
                return null;
            }
            
            if (Consume(source, TokenTag.Equal))
            {
                initializer.Value = Expression(source);
                if (initializer.Value == null)
                    Report.Error($"{source.Position} Expression expected.");
                else
                    literal.Initializers.Add(initializer);

                while (Consume(source, TokenTag.Comma))
                {
                    initializer = new FieldInitializer();
                    initializer.Name = Identifier(source);
                    if (initializer.Name == null)
                        break;

                    if (!Consume(source, TokenTag.Equal))
                        Report.Error($"{source.Position} = expected.");

                    initializer.Value = Expression(source);
                    if (initializer.Value == null)
                        Report.Error($"{source.Position} Expression expected.");
                    else
                        literal.Initializers.Add(initializer);
                }

                if (!Consume(source, TokenTag.End))
                {
                    source.Index = start;
                    return null;
                }
                else
                    return literal;
            }
            else
            {
                source.Index = start;
                return null;
            }
        }

        private static Expression ArrayLiteral(Source source)
        {
            if (Consume(source, TokenTag.LeftSquareBracket))
            {
                Expect(source, TokenTag.RightSquareBracket, "]");
                var type = Type(source);
                if (type == null)
                    Report.Error($"{source.Position} Type expected.");

                return new ArrayLiteral()
                {
                    ElementType = type
                };
            }
            else
                return null;
        }

        private static Expression ParentherizedExpression(Source source)
        {
            if (Consume(source, TokenTag.LeftParentheses))
            {
                var expression = Expression(source);
                if (expression == null)
                    Report.Error($"{source.Position} Expression expected.");
                else
                    Expect(source, TokenTag.RightParentheses, ")");

                return expression;
            }
            else
                return null;
        }

        private static Type Type(Source source)
        {
            var firstPointerOrArray = default(Type);
            var lastPointerOrArray = default(Type);

            while (true)
            {
                if (Consume(source, TokenTag.AddressOf))
                {
                    if (firstPointerOrArray == null)
                    {
                        firstPointerOrArray = new Pointer();
                        lastPointerOrArray = firstPointerOrArray;
                    }
                    else if (lastPointerOrArray is Pointer pointer)
                    {
                        pointer.To = new Pointer();
                        lastPointerOrArray = pointer.To;
                    }
                    else if (lastPointerOrArray is Array array)
                    {
                        array.Of = new Array();
                        lastPointerOrArray = array.Of;
                    }
                }
                else if (Consume(source, TokenTag.LeftSquareBracket))
                {
                    Expect(source, TokenTag.RightSquareBracket, "]");
                    if (firstPointerOrArray == null)
                    {
                        firstPointerOrArray = new Array();
                        lastPointerOrArray = firstPointerOrArray;
                    }
                    else if (lastPointerOrArray is Pointer pointer)
                    {
                        pointer.To = new Array();
                        lastPointerOrArray = pointer.To;
                    }
                    else if (lastPointerOrArray is Array array)
                    {
                        array.Of = new Array();
                        lastPointerOrArray = array.Of;
                    }
                }
                else
                    break;
            }

            if (Consume(source, TokenTag.Function))
                Report.Error($"{source.Position} Function types not supported.");

            var unresolved = new UnresolvedType();
            unresolved.Identifier = Identifier(source);

            if (unresolved.Identifier == null)
            {
                if (firstPointerOrArray != null)
                    Report.Error($"{source.Position} Identifier expected.");
                
                return null;
            }
            else
            {
                if (lastPointerOrArray == null)
                    return unresolved;
                else
                {
                    if (lastPointerOrArray is Pointer pointer)
                        pointer.To = unresolved;
                    else if (lastPointerOrArray is Array array)
                        array.Of = unresolved;

                    return firstPointerOrArray;
                }
            }
        }

        private static bool Consume(Source source, TokenTag tag)
        {
            if (source.Current?.Tag == tag)
            {
                source.Index++;
                return true;
            }
            else
                return false;
        }

        private static Identifier Identifier(Source source)
        {
            if (source.Current.Tag == TokenTag.Identifier)
            {
                var token = source.Tokens[source.Index++];
                return new Identifier()
                {
                    Start = token.Start,
                    End = token.End,
                    Value = token.Value
                };
            }
            else
                return null;
        }

        private static Boolean Boolean(Source source)
        {
            if (source.Current.Tag == TokenTag.True || source.Current.Tag == TokenTag.False)
            {
                var token = source.Tokens[source.Index++];
                return new Boolean()
                {
                    Start = token.Start,
                    End = token.End,
                    Value = token.Value
                };
            }
            else
                return null;
        }

        private static NullLiteral NullLiteral(Source source)
        {
            if (source.Current.Tag == TokenTag.Null)
            {
                var token = source.Tokens[source.Index++];
                return new NullLiteral()
                {
                    Start = token.Start,
                    End = token.End
                };
            }
            else
                return null;
        }

        private static String String(Source source)
        {
            if (source.Current.Tag == TokenTag.String)
            {
                var token = source.Tokens[source.Index++];
                return new String()
                {
                    Start = token.Start,
                    End = token.End,
                    Value = token.Value
                };
            }
            else
                return null;
        }

        private static Operator Operator(Source source)
        {
            if (source.Current.Tag >= TokenTag.LogicalOr && source.Current.Tag <= TokenTag.RightShift)
            {
                var token = source.Tokens[source.Index++];
                return new Operator()
                {
                    Start = token.Start,
                    End = token.End,
                    Tag = (OperatorTag)(token.Tag - TokenTag.LogicalOr)
                };
            }
            else
                return null;
        }

        private static Number Number(Source source)
        {
            if (source.Current.Tag >= TokenTag.I8 && source.Current.Tag <= TokenTag.NumberToBeInfered)
            {
                var token = source.Tokens[source.Index++];
                return new Number()
                {
                    Start = token.Start,
                    End = token.End,
                    Tag = (NumberTag)token.Tag,
                    Value = token.Value
                };
            }
            else
                return null;
        }

        private static void Expect(Source source, TokenTag tag, string missing)
        {
            if (!Consume(source, tag))
                Report.Error($"{source.Position} {missing} expected.");
        }
    }
}