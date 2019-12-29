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
                Report.Error($"{source.Position} Identifier expected.");

            while (Consume(source, "use"))
            {
                var use = Identifier(source);
                if (use == null)
                    Report.Error($"{source.Position} Identifier expected.");

                file.Uses.Add(use);
            }

            while (!source.EndOfText)
            {
                var isPublic = Consume(source, "public");

                var function = FunctionDeclaration(source, isPublic);
                if (function != null)
                    file.Functions.Add(function);

                var proposition = PropositionDeclaration(source);
                if (proposition != null)
                    file.Propositions.Add(proposition);

                var compound = CompoundDeclaration(source, isPublic);
                if (compound != null)
                    file.Compounds.Add(compound);

                var enumeration = EnumerationDeclaration(source, isPublic);
                if (enumeration != null)
                    file.Enumerations.Add(enumeration);

                var expression = default(Expression);
                if (Consume(source, "ctfe"))
                {
                    expression = Expression(source);
                    if (expression == null)
                        Report.Error($"{source.Position} Expression expected.");
                    else
                        file.Ctfe.Add(expression);
                }

                if (function == null && proposition == null && compound == null && enumeration == null && expression == null)
                    Report.Error($"{source.Position} Function, proposition, structure, union, enumeration or CTFE expression expected.");
            }

            program.Files.Add(file);
        }

        private static FunctionDeclaration FunctionDeclaration(Source source, bool isPublic)
        {
            if (Consume(source, "function"))
            {
                var name = Identifier(source);
                if (name == null)
                    Report.Error("Identifier expected.");

                var declaration = new FunctionDeclaration();
                declaration.IsPublic = isPublic;
                declaration.Name = name;

                if (Consume(source, "input"))
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
                    } while (Consume(source, ","));
                }

                if (Consume(source, "output"))
                {
                    var types = new List<Type>();
                    do
                    {
                        var identifier = Identifier(source);
                        if (identifier == null)
                            Report.Error("Identifier expected.");
                        else
                            types.Add(new UnresolvedType()
                            {
                                Identifier = identifier
                            });
                    } while (Consume(source, ","));

                    if (types.Count == 1)
                        declaration.Output = types[0];
                    else if (types.Count > 1)
                        declaration.Output = new TupleType()
                        {
                            Types = types
                        };
                }

                declaration.Body = CompoundStatement(source);
                Expect(source, "end");

                return declaration;
            }
            else
                return null;
        }

        private static CompoundStatement PropositionDeclaration(Source source)
        {
            if (Consume(source, "proposition"))
            {
                var body = CompoundStatement(source);
                Expect(source, "end");

                return body;
            }
            else
                return null;
        }

        private static CompoundDeclaration CompoundDeclaration(Source source, bool isPublic)
        {
            CompoundTypeTag tag;
            if (Consume(source, "structure"))
                tag = CompoundTypeTag.Structure;
            else if (Consume(source, "union"))
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

            Expect(source, "end");

            return declaration;
        }

        private static EnumerationDeclaration EnumerationDeclaration(Source source, bool isPublic)
        {
            if (Consume(source, "enumeration"))
            {
                var declaration = new EnumerationDeclaration();
                declaration.IsPublic = isPublic;
                declaration.Name = Identifier(source);
                if (declaration.Name == null)
                    Report.Error($"{source.Position} Identifier expected.");

                Expect(source, "of");
                
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
                        var constant = new EnumerationConstant()
                        {
                            Name = name
                        };

                        if (Consume(source, "="))
                        {
                            var value = (Number)Number(source);
                            if (value == null || value.Value.Contains("."))
                                Report.Error($"{source.Position} Integer expected.");
                            else
                                constant.Value = value;
                        }

                        declaration.Constants.Add(constant);
                    }
                }

                Expect(source, "end");

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
                var assignment = new AssignmentStatement();
                assignment.Operator = new Operator();

                var operatorStart = source.Index;
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

                UpdatePosition(source, operatorStart);
                assignment.Left = left;
                assignment.Operator.Start = source.Position.Copy();

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

        private static Statement IfStatement(Source source)
        {
            if (Consume(source, "if"))
            {
                var block = new ConditionalBlock();
                
                block.Assignment = (AssignmentStatement)AssignmentStatement(source);
                if (block.Assignment != null)
                    Expect(source, ";");

                block.Condition = Expression(source);
                if (block.Condition == null)
                    Report.Error($"{source.Position} Expression expected.");

                block.Body = CompoundStatement(source);

                var statement = new IfStatement();
                statement.Blocks.Add(block);

                while (Consume(source, "else"))
                {
                    block = new ConditionalBlock();
                    if (Consume(source, "if"))
                    {
                        block.Assignment = (AssignmentStatement)AssignmentStatement(source);
                        if (block.Assignment != null)
                            Expect(source, ";");

                        block.Condition = Expression(source);
                        if (block.Condition == null)
                            Report.Error($"{source.Position} Expression expected.");
                    }

                    block.Body = CompoundStatement(source);
                    statement.Blocks.Add(block);

                    if (block.Condition == null)
                        break;
                }

                Expect(source, "end");

                return statement;
            }
            else
                return null;
        }

        private static Statement ForStatement(Source source)
        {
            if (Consume(source, "for"))
            {
                var statement = new ForStatement();
                statement.Assignment = (AssignmentStatement)AssignmentStatement(source);
                if (statement.Assignment == null)
                    Report.Error($"{source.Position} Assignment expected.");

                Expect(source, ";");

                statement.Condition = Expression(source);
                if (statement.Condition == null)
                    Report.Error($"{source.Position} Expression expected.");

                Expect(source, ";");
                statement.Post = Expressions(source);

                statement.Body = CompoundStatement(source);
                Expect(source, "end");

                return statement;
            }
            else
                return null;
        }

        private static Statement WhileStatement(Source source)
        {
            if (Consume(source, "while"))
            {
                var statement = new WhileStatement();
                statement.Assignment = (AssignmentStatement)AssignmentStatement(source);

                if (statement.Assignment != null)
                    Expect(source, ";");

                statement.Condition = Expression(source);
                if (statement.Condition == null)
                    Report.Error($"{source.Position} Expression expected.");

                statement.Body = CompoundStatement(source);
                Expect(source, "end");

                return statement;
            }
            else
                return null;
        }

        private static Statement BreakStatement(Source source)
        {
            var start = source.Position.Copy();
            if (Consume(source, "break"))
            {
                return new BreakStatement()
                {
                    Start = start
                };
            }
            else
                return null;
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
            var start = source.Position.Copy();
            if (Consume(source, "size"))
            {
                Expect(source, "of");

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
            else if (Consume(source, "&"))
            {
                var expression = PostfixExpression(source);
                if (expression == null)
                    Report.Error($"{source.Position} Expression expected.");

                return new AddressOf()
                {
                    Start = start,
                    Expression = expression,
                    End = expression.End
                };
            }
            else if (Consume(source, "*"))
            {
                var expression = PostfixExpression(source);
                if (expression == null)
                    Report.Error($"{source.Position} Expression expected.");

                return new Dereference()
                {
                    Start = start,
                    Expression = expression,
                    End = expression.End
                };
            }
            else
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
                else if (Consume(source, "."))
                {
                    var field = PostfixExpression(source);
                    if (field == null)
                        Report.Error($"{source.Position} Expression expected.");

                    return new DotExpression()
                    {
                        Start = expression.Start,
                        Structure = expression,
                        Field = field,
                        End = field.End
                    };
                }
                else if (Consume(source, "++"))
                    return new PostfixIncrement()
                    {
                        Expression = expression
                    };
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
                   StructureLiteral(source) ??
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

        private static Expression Boolean(Source source)
        {
            var start = source.Index;
            if (Consume(source, "true"))
            {
                UpdatePosition(source, start);
                var boolean = new Boolean();

                boolean.Start = source.Position.Copy();
                boolean.Value = "true";
                boolean.End = source.Position.Copy();
                boolean.End.Column += boolean.Value.Length;

                return boolean;
            }
            else if (Consume(source, "false"))
            {
                UpdatePosition(source, start);
                var boolean = new Boolean();

                boolean.Start = source.Position.Copy();
                boolean.Value = "false";
                boolean.End = source.Position.Copy();
                boolean.End.Column += boolean.Value.Length;

                return boolean;
            }
            else
                return null;
        }

        private static Expression StructureLiteral(Source source)
        {
            var start = source.Index;
            var literal = new CompoundLiteral();
            literal.Structure = Identifier(source);

            if (literal.Structure == null)
                return null;

            var initializer = new FieldInitializer();
            initializer.Name = Identifier(source);
            if (initializer.Name == null)
            {
                source.Index = start;
                return null;
            }

            if (Consume(source, "="))
            {
                initializer.Value = Expression(source);
                if (initializer.Value == null)
                    Report.Error($"{source.Position} Expression expected.");
                else
                    literal.Initializers.Add(initializer);

                while (Consume(source, ","))
                {
                    initializer = new FieldInitializer();
                    initializer.Name = Identifier(source);
                    if (initializer.Name == null)
                        break;
                    else
                        Expect(source, "=");

                    initializer.Value = Expression(source);
                    if (initializer.Value == null)
                        Report.Error($"{source.Position} Expression expected.");
                    else
                        literal.Initializers.Add(initializer);
                }

                Expect(source, "end");

                return literal;
            }
            else
            {
                source.Index = start;
                return null;
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

        private static Type Type(Source source)
        {
            var firstPointerOrArray = default(Type);
            var lastPointerOrArray = default(Type);

            while (true)
            {
                if (Consume(source, "*"))
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
                }
                else
                    break;
            }

            if (Consume(source, "function"))
                Report.Error($"{source.Position} Function types not supported.");

            var unresolved = new UnresolvedType()
            {
                Identifier = Identifier(source)
            };

            if (unresolved.Identifier == null)
            {
                if (firstPointerOrArray != null)
                    Report.Error($"{source.Position} Identifier expected.");
                
                return null;
            }
            else
            {
                if (lastPointerOrArray is Pointer pointer)
                {
                    pointer.To = unresolved;
                    return firstPointerOrArray;
                }
                else
                    return unresolved;
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