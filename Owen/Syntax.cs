﻿using System.Collections.Generic;

internal static class Syntax
{
    private static List<Token> Tokenize(string path, string text)
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
            else if (text.Length >= index + 1 && text[index] == '-' && text[index + 1] >= '0' && text[index + 1] <= '9' || text.Length != index && text[index] >= '0' && text[index] <= '9')
            {
                if (text[index] == '-')
                {
                    index++;
                    position.Column++;
                }

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
                    Value = text.Substring(startIndex + 1, index - startIndex - 2),
                    Tag = TokenTag.String
                });
            }
            else
                Report.Error($"{position} Unexpected character '{text[index]}'.");
        }

        return tokens;
    }

    public static File Parse(string path, string text, List<string> versions)
    {
        var file = new File();
        file.Path = path;

        var source = new Source();
        source.Tokens = Tokenize(path, text);

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
            var isExternal = Consume(source, TokenTag.External);

            if (Consume(source, TokenTag.Function))
            {
                var declaration = new FunctionDeclaration();
                declaration.Name = Name(source);
                if (declaration.Name == null)
                    Report.Error($"{source.Position} Identifier expected.");

                declaration.IsPublic = isPublic;
                if (Consume(source, TokenTag.Generalize))
                {
                    do
                    {
                        var identifier = Identifier(source);
                        if (identifier == null)
                            Report.Error($"{source.Position} Identifier expected.");

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

                        argument.Name = Name(source);
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

                        types.Add(type);
                    } while (Consume(source, TokenTag.Comma));

                    if (types.Count == 1)
                        declaration.Output = types[0];
                    else if (types.Count > 1)
                        declaration.Output = new TupleType() { Types = types };
                }

                if (isExternal)
                {
                    var expression = Expression(source);
                    if (expression is String s)
                        declaration.Library = s;
                    else
                        Report.Error($"{source.Position} String expected.");

                    declaration.IsExternal = true;
                    declaration.Body = new CompoundStatement();
                }
                else
                {
                    declaration.Body = CompoundStatement(source);
                    Expect(source, TokenTag.End, "end");
                }

                file.Functions.Add(declaration);
            }
            else if (isExternal)
                Report.Error($"{source.Position} Function declaration expected.");
            else if (Consume(source, TokenTag.Proposition))
            {
                file.Propositions.Add(CompoundStatement(source));
                Expect(source, TokenTag.End, "end");
            }
            else if (source.Current?.Tag == TokenTag.Union || source.Current?.Tag == TokenTag.Structure)
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
                    Name = Name(source)
                };

                if (declaration.Name == null)
                    Report.Error($"{source.Position} Identifier expected.");

                while (true)
                {
                    var field = new Field();
                    field.Type = Type(source);
                    if (field.Type == null)
                        break;

                    field.Name = Name(source);
                    if (field.Name == null)
                        Report.Error($"{source.Position} Identifier expected.");

                    declaration.Fields.Add(field);
                }

                Expect(source, TokenTag.End, "end");

                file.Compounds.Add(declaration);
            }
            else if (Consume(source, TokenTag.Enumeration))
            {
                var declaration = new EnumerationDeclaration();
                declaration.IsPublic = isPublic;
                declaration.Name = Name(source);
                if (declaration.Name == null)
                    Report.Error($"{source.Position} Identifier expected.");

                Expect(source, TokenTag.Of, "of");

                declaration.Type = Type(source);
                if (declaration.Type == null)
                    Report.Error($"{source.Position} Type expected.");

                while (true)
                {
                    var name = Name(source);
                    if (name == null)
                        break;

                    var constant = new EnumerationConstant();
                    constant.Name = name;

                    if (Consume(source, TokenTag.Equal))
                    {
                        var value = Expression(source);
                        if (value is Number number)
                            constant.Value = number;
                        else
                            Report.Error($"{source.Position} Integer expected.");
                    }

                    declaration.Constants.Add(constant);
                }

                Expect(source, TokenTag.End, "end");

                file.Enumerations.Add(declaration);
            }
            else if (Consume(source, TokenTag.Ctfe))
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

        return file;
    }

    private static CompoundStatement CompoundStatement(Source source)
    {
        var compound = new CompoundStatement();
        while (true)
        {
            var assigmentStatement = AssignmentStatement(source);
            if (assigmentStatement != null)
            {
                compound.Statements.Add(assigmentStatement);
                continue;
            }

            var expression = Expression(source);
            if (expression != null)
                compound.Statements.Add(new ExpressionStatement() { Expression = expression });
            else if (Consume(source, TokenTag.If))
            {
                var branch = new ConditionalBranch();

                branch.Assignment = AssignmentStatement(source);
                if (branch.Assignment != null)
                    Expect(source, TokenTag.Semicolon, ";");

                branch.Condition = Expression(source);
                if (branch.Condition == null)
                    Report.Error($"{source.Position} Expression expected.");

                branch.Body = CompoundStatement(source);

                var statement = new IfStatement();
                statement.Branches.Add(branch);

                while (Consume(source, TokenTag.Else))
                {
                    branch = new ConditionalBranch();
                    if (Consume(source, TokenTag.If))
                    {
                        branch.Assignment = AssignmentStatement(source);
                        if (branch.Assignment != null)
                            Expect(source, TokenTag.Semicolon, ";");

                        branch.Condition = Expression(source);
                        if (branch.Condition == null)
                            Report.Error($"{source.Position} Expression expected.");
                    }

                    branch.Body = CompoundStatement(source);
                    statement.Branches.Add(branch);

                    if (branch.Condition == null)
                        break;
                }

                Expect(source, TokenTag.End, "end");

                compound.Statements.Add(statement);
            }
            else if (Consume(source, TokenTag.For))
            {
                var statement = new ForStatement();
                statement.Assignment = AssignmentStatement(source);
                if (statement.Assignment == null)
                    Report.Error($"{source.Position} Assignment expected.");

                Expect(source, TokenTag.Semicolon, ";");

                statement.Condition = Expression(source);
                if (statement.Condition == null)
                    Report.Error($"{source.Position} Expression expected.");

                Expect(source, TokenTag.Semicolon, ";");
                statement.Post = AssignmentStatement(source);

                statement.Body = CompoundStatement(source);
                Expect(source, TokenTag.End, "end");

                compound.Statements.Add(statement);
            }
            else if (Consume(source, TokenTag.While))
            {
                var statement = new WhileStatement();
                statement.Assignment = AssignmentStatement(source);

                if (statement.Assignment != null)
                    Expect(source, TokenTag.Semicolon, ";");

                statement.Condition = Expression(source);
                if (statement.Condition == null)
                    Report.Error($"{source.Position} Expression expected.");

                statement.Body = CompoundStatement(source);
                Expect(source, TokenTag.End, "end");

                compound.Statements.Add(statement);
            }
            else if (Consume(source, TokenTag.Break))
                compound.Statements.Add(new BreakStatement() { Start = source.Tokens[source.Index - 1].Start });
            else if (Consume(source, TokenTag.Return))
            {
                compound.Statements.Add(new ReturnStatement()
                {
                    Start = source.Tokens[source.Index - 1].Start,
                    Expressions = Expressions(source)
                });
            }
            else if (Consume(source, TokenTag.Assert))
            {
                var assertion = Expression(source);
                if (assertion == null)
                    Report.Error($"{source.Position} Assertion expected.");

                compound.Statements.Add(new AssertStatement() { Assertion = assertion });
            }
            else
                break;
        }

        return compound;
    }

    private static AssignmentStatement AssignmentStatement(Source source)
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

            source.Index = start;
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

                expressions.Add(expression);
            }
        }

        return expressions;
    }

    private static Expression Expression(Source source, int minimumPrecedence = 0)
    {
        // Primary and prefix expressions.
        var left = default(Expression);
        if (source.Current.Tag >= TokenTag.I8 && source.Current.Tag <= TokenTag.NumberToBeInfered)
        {
            var token = source.Tokens[source.Index++];
            left = new Number()
            {
                Start = token.Start,
                End = token.End,
                Tag = (NumberTag)token.Tag,
                Text = token.Value
            };
        }
        else if (source.Current.Tag == TokenTag.True || source.Current.Tag == TokenTag.False)
        {
            var token = source.Tokens[source.Index++];
            left = new Boolean()
            {
                Start = token.Start,
                End = token.End,
                Value = token.Value
            };
        }
        else if (source.Current.Tag == TokenTag.String)
        {
            var token = source.Tokens[source.Index++];
            left = new String()
            {
                Start = token.Start,
                End = token.End,
                Value = token.Value
            };
        }
        else if (source.Current.Tag == TokenTag.Null)
        {
            var token = source.Tokens[source.Index++];
            left = new NullLiteral()
            {
                Start = token.Start,
                End = token.End
            };
        }
        else if (source.Current.Tag == TokenTag.Identifier)
        {
            var identifier = Identifier(source);
            var start = source.Index;

            var compoundLiteral = new CompoundLiteral();
            compoundLiteral.Structure = identifier;
            compoundLiteral.Start = compoundLiteral.Structure.Start;

            var initializer = new FieldInitializer();
            initializer.Name = Name(source);
            if (initializer.Name == null)
            {
                left = identifier;
                source.Index = start;
            }
            else if (Consume(source, TokenTag.Equal))
            {
                initializer.Value = Expression(source);
                if (initializer.Value == null)
                    Report.Error($"{source.Position} Expression expected.");
                else
                    compoundLiteral.Initializers.Add(initializer);

                while (Consume(source, TokenTag.Comma))
                {
                    initializer = new FieldInitializer();
                    initializer.Name = Name(source);
                    if (initializer.Name == null)
                        break;

                    if (!Consume(source, TokenTag.Equal))
                        Report.Error($"{source.Position} = expected.");

                    initializer.Value = Expression(source);
                    if (initializer.Value == null)
                        Report.Error($"{source.Position} Expression expected.");
                    else
                        compoundLiteral.Initializers.Add(initializer);
                }

                if (Consume(source, TokenTag.End))
                    left = compoundLiteral;
                else
                {
                    left = identifier;
                    source.Index = start;
                }
            }
            else
            {
                left = identifier;
                source.Index = start;
            }
        }
        else if (Consume(source, TokenTag.LeftSquareBracket))
        {
            Expect(source, TokenTag.RightSquareBracket, "]");
            var type = Type(source);
            if (type == null)
                Report.Error($"{source.Position} Type expected.");

            left = new ArrayLiteral() { ElementType = type };
        }
        else if (Consume(source, TokenTag.LeftParentheses))
        {
            left = Expression(source);
            if (left == null)
                Report.Error($"{source.Position} Expression expected.");
            else
                Expect(source, TokenTag.RightParentheses, ")");
        }
        else
        {
            var start = source.Position;
            if (Consume(source, TokenTag.Not))
            {
                left = Expression(source, int.MaxValue);
                if (left == null)
                    Report.Error($"{source.Position} Expression expected.");

                left = new Not()
                {
                    Start = start,
                    Expression = left
                };
            }
            else if (Consume(source, TokenTag.Minus))
            {
                left = Expression(source, int.MaxValue);
                if (left == null)
                    Report.Error($"{source.Position} Expression expected.");

                left = new Negate()
                {
                    Start = start,
                    Expression = left
                };
            }
            else if (Consume(source, TokenTag.Size))
            {
                Expect(source, TokenTag.Of, "of");

                var type = Type(source);
                if (type == null)
                    Report.Error($"{source.Position} Type expected.");

                left = new SizeOf()
                {
                    Start = start,
                    TypeBeingSizedUp = type
                };
            }
            else if (Consume(source, TokenTag.AddressOf))
            {
                left = Expression(source, int.MaxValue);
                if (left == null)
                    Report.Error($"{source.Position} Expression expected.");

                left = new AddressOf()
                {
                    Start = start,
                    Expression = left
                };
            }
            else if (Consume(source, TokenTag.Dereference))
            {
                left = Expression(source, int.MaxValue);
                if (left == null)
                    Report.Error($"{source.Position} Expression expected.");

                left = new Dereference()
                {
                    Start = start,
                    Expression = left
                };
            }
            else if (Consume(source, TokenTag.Cast))
            {
                Expect(source, TokenTag.LeftParentheses, "(");
                var type = Type(source);
                if (type == null)
                    Report.Error($"{source.Position} Type expected.");

                Expect(source, TokenTag.RightParentheses, ")");
                left = new Cast()
                {
                    Start = start,
                    Expression = Expression(source, int.MaxValue),
                    To = type
                };
            }
        }

        if (left == null)
            return null;

        // Postfix expressions
        {
            var startOfInput = default(Position);
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

                        left = new Call()
                        {
                            Generics = generics,
                            Start = startOfInput,
                            Reference = left,
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

                left = new Call()
                {
                    Generics = new List<Type>(),
                    Start = startOfInput,
                    Reference = left,
                    Arguments = arguments
                };
            }
            else if (Consume(source, TokenTag.Dot))
            {
                var field = Expression(source, int.MaxValue);
                if (field == null)
                    Report.Error($"{source.Position} Expression expected.");
                else if (field is Identifier || field is FieldAccess)
                    left = new FieldAccess()
                    {
                        Start = left.Start,
                        Object = left,
                        Field = field
                    };
                else
                    Report.Error($"{field.Start} Field expected.");
            }
            else if (Consume(source, TokenTag.LeftSquareBracket))
            {
                var position = Expression(source);
                if (position == null)
                    Report.Error($"{source.Position} Expression expected.");

                Expect(source, TokenTag.RightSquareBracket, "]");
                left = new ArrayAccess()
                {
                    Start = startOfInput,
                    Array = left,
                    Position = position
                };
            }
        }

        // Binary expressions.
        while (true)
        {
            var precedence = 0;
            if (source.Current?.Tag == TokenTag.LogicalOr)
                precedence = 0;
            else if (source.Current?.Tag == TokenTag.LogicalAnd)
                precedence = 1;
            else if (source.Current?.Tag >= TokenTag.EqualEqual && source.Current?.Tag <= TokenTag.GreaterThan)
                precedence = 2;
            else if (source.Current?.Tag >= TokenTag.Plus && source.Current?.Tag <= TokenTag.BitwiseXor)
                precedence = 3;
            else if (source.Current?.Tag >= TokenTag.Multiply && source.Current?.Tag <= TokenTag.RightShift)
                precedence = 4;
            else
                break;

            if (precedence < minimumPrecedence)
                break;

            var startOfOperator = source.Index;
            var op = Operator(source);

            var right = Expression(source, precedence + 1);
            if (right == null)
            {
                if (precedence == 0)
                    Report.Error($"{source.Position} Expression expected.");
                else
                    source.Index = startOfOperator;
            }
            else
            {
                left = new BinaryExpression()
                {
                    Start = left.Start,
                    End = right.End,
                    Left = left,
                    Operator = op,
                    Right = right
                };
            }
        }

        return left;
    }

    private static Type Type(Source source)
    {
        var firstPointerOrArray = default(Type);
        var lastPointerOrArray = default(Type);

        // this seems like a really dumb way to parse types.
        while (true)
        {
            if (Consume(source, TokenTag.AddressOf))
            {
                if (firstPointerOrArray == null)
                {
                    firstPointerOrArray = new PointerType();
                    lastPointerOrArray = firstPointerOrArray;
                }
                else if (lastPointerOrArray is PointerType pointer)
                {
                    pointer.To = new PointerType();
                    lastPointerOrArray = pointer.To;
                }
                else if (lastPointerOrArray is ArrayType array)
                {
                    array.Of = new ArrayType();
                    lastPointerOrArray = array.Of;
                }
            }
            else if (Consume(source, TokenTag.LeftSquareBracket))
            {
                Expect(source, TokenTag.RightSquareBracket, "]");
                if (firstPointerOrArray == null)
                {
                    firstPointerOrArray = new ArrayType();
                    lastPointerOrArray = firstPointerOrArray;
                }
                else if (lastPointerOrArray is PointerType pointer)
                {
                    pointer.To = new ArrayType();
                    lastPointerOrArray = pointer.To;
                }
                else if (lastPointerOrArray is ArrayType array)
                {
                    array.Of = new ArrayType();
                    lastPointerOrArray = array.Of;
                }
            }
            else
                break;
        }

        if (Consume(source, TokenTag.Function))
            Report.InternalError($"{source.Position} Function types yet not supported.");

        var unresolved = new UnresolvedType();
        unresolved.Name = Name(source);

        if (unresolved.Name == null)
        {
            if (firstPointerOrArray != null)
                Report.Error($"{source.Position} Identifier expected.");

            return null;
        }
        else if (lastPointerOrArray == null)
            return unresolved;
        else
        {
            if (lastPointerOrArray is PointerType pointer)
                pointer.To = unresolved;
            else if (lastPointerOrArray is ArrayType array)
                array.Of = unresolved;

            return firstPointerOrArray;
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

    private static Name Name(Source source)
    {
        if (source.Current.Tag == TokenTag.Identifier)
        {
            var token = source.Tokens[source.Index++];
            return new Name()
            {
                Start = token.Start,
                End = token.End,
                Value = token.Value
            };
        }

        return null;
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

        return null;
    }

    private static void Expect(Source source, TokenTag tag, string missing)
    {
        if (!Consume(source, tag))
            Report.Error($"{source.Position} {missing} expected.");
    }

    private sealed class Source
    {
        public int Index;
        public List<Token> Tokens;
        public Token Current => EndOfTokens ? null : Tokens[Index];
        public Position Position => Current?.Start;
        public bool EndOfTokens => Tokens.Count == Index;
    }
}