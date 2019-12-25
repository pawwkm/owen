using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;

namespace Owen
{
    internal static class D
    {
        private static int tempNamesCreated;

        private static string CreateTempName()
        {
            return $"å{tempNamesCreated++}";
        }

        public static void Generate(Program program, string output)
        {
            var root = Path.Combine(Path.GetTempPath(), "Owen");
            if (Directory.Exists(root))
            {
                foreach (var path in Directory.EnumerateFiles(root, "*.*", SearchOption.AllDirectories))
                    System.IO.File.Delete(path);

                Directory.Delete(root);
            }

            Directory.CreateDirectory(root);
            for (var i = 0; i < program.Files.Count; i++)
            {   
                System.IO.File.WriteAllText
                (
                    Path.Combine(root, $"_{i}.d"), // dmd does not like file names starting with a digit.
                    Generate(program.Files[i]),
                    Encoding.UTF8
                );
            }

            try
            {
                var compiler = new Process();
                compiler.StartInfo = new ProcessStartInfo("dmd.exe", $"-release -betterC -O -noboundscheck -check=assert=on \"-of{output}\" {string.Join(" ", Enumerable.Range(0, program.Files.Count).Select(i => $"_{i}.d"))}")
                {
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    UseShellExecute = false,
                    CreateNoWindow = true,
                    WorkingDirectory = root
                };

                compiler.Start();
                compiler.WaitForExit();

                if (compiler.ExitCode != 0)
                    Report.Error($"dmd: {compiler.StandardError.ReadToEnd()}");
            }
            catch (Win32Exception)
            {
                Report.Error("Is dmd installed and set in the PATH variable?");
            }
        }

        private static string Generate(File file)
        {
            var builder = new StringBuilder();

            builder.Append("import std.typecons; ");
            foreach (var enumeration in file.Enumerations)
                Generate(enumeration, builder);

            foreach (var compound in file.Compounds)
                Generate(compound, builder);

            foreach (var function in file.Functions)
                Generate(function, builder);

            return builder.ToString();
        }

        private static void Generate(EnumerationDeclaration enumeration, StringBuilder builder)
        {
            builder.Append("enum ");
            builder.Append(enumeration.Name.Value);
            builder.Append(':');
            Generate(enumeration.Type, builder);
            builder.Append('{');

            for (var i = 0; i < enumeration.Constants.Count; i++)
            {
                builder.Append(enumeration.Constants[i].Name.Value);
                builder.Append('=');
                builder.Append(enumeration.Constants[i].Value.Value);

                if (i + 1 != enumeration.Constants.Count)
                    builder.Append(',');
            }

            builder.Append('}');
        }

        private static void Generate(CompoundDeclaration compound, StringBuilder builder)
        {
            if (compound.Tag == CompoundTypeTag.Struct)
                builder.Append("struct ");
            else
                builder.Append("union ");

            builder.Append(compound.Name.Value);
            builder.Append('{');
            
            foreach (var field in compound.Fields)
            {
                Generate(field.Type, builder);
                builder.Append(field.Name.Value);
                builder.Append(';');
            }

            builder.Append('}');
        }

        private static void Generate(FunctionDeclaration function, StringBuilder builder)
        {
            if (function.Name.Value == "main")
                builder.Append("extern(C) ");

            Generate(function.Output, builder);
            builder.Append(function.Name.Value);
            builder.Append('(');

            for (var i = 0; i < function.Input.Count; i++)
            {
                Generate(function.Input[i].Type, builder);
                builder.Append(' ');
                builder.Append(function.Input[i].Name.Value);

                if (i + 1 != function.Input.Count)
                    builder.Append(',');
            }

            builder.Append(')');
            Generate(function.Body, builder);
        }

        private static void Generate(Type type, StringBuilder builder)
        {
            if (type is PrimitiveType primitive)
            {
                switch (primitive.Tag)
                {
                    case PrimitiveTypeTag.I8:
                        builder.Append("byte ");
                        break;
                    case PrimitiveTypeTag.I16:
                        builder.Append("short ");
                        break;
                    case PrimitiveTypeTag.I32:
                        builder.Append("int ");
                        break;
                    case PrimitiveTypeTag.I64:
                        builder.Append("long ");
                        break;
                    case PrimitiveTypeTag.U8:
                        builder.Append("ubyte ");
                        break;
                    case PrimitiveTypeTag.U16:
                        builder.Append("ushort ");
                        break;
                    case PrimitiveTypeTag.U32:
                        builder.Append("uint ");
                        break;
                    case PrimitiveTypeTag.U64:
                        builder.Append("ulong ");
                        break;
                    case PrimitiveTypeTag.F32:
                        builder.Append("float ");
                        break;
                    case PrimitiveTypeTag.F64:
                        builder.Append("double ");
                        break;
                    case PrimitiveTypeTag.Bool:
                        builder.Append("bool ");
                        break;
                    default:
                        Report.Error($"Cannot translate {type} to D.");
                        break;
                }
            }
            else if (type is Pointer pointer)
            {
                Generate(pointer.To, builder);
                builder.Append('*');
            }
            else if (type == null)
                builder.Append("void ");
            else if (type is TupleType tuple)
            {
                builder.Append("Tuple!(");
                for (var i = 0; i < tuple.Types.Count; i++)
                {
                    Generate(tuple.Types[i], builder);
                    if (i + 1 != tuple.Types.Count)
                        builder.Append(',');
                }

                builder.Append(')');
            }
            else if (type is CompoundDeclaration compound)
            {
                builder.Append(compound.Name.Value);
                builder.Append(' ');
            }
            else
                Report.Error($"Cannot translate {type} to D.");
        }

        private static void Generate(Statement statement, StringBuilder builder)
        {
            if (statement is CompoundStatement compound)
            {
                builder.Append('{');
                foreach (var s in compound.Statements)
                    Generate(s, builder);

                builder.Append('}');
            }
            else if (statement is AssignmentStatement assignment)
            {
                if (assignment.Right[0] is Call call && call.Declaration.Output is TupleType tuple)
                {
                    foreach (var left in assignment.Left)
                    {
                        if (left is VariableDeclaration declaration)
                        {
                            Generate(declaration.Type, builder);
                            builder.Append(declaration.Variable.Value);
                            builder.Append(';');
                        }
                        else if (!(left is Identifier))
                            Report.Error($"{left.Start} Cannot translate assignment to {left.GetType().Name} to D.");
                    }

                    var temp = CreateTempName();
                    builder.Append($"auto {temp} = ");
                    Generate(call, builder);
                    builder.Append(';');

                    for (var i = 0; i < assignment.Left.Count; i++)
                    {
                        if (assignment.Left[i] is VariableDeclaration declaration)
                            builder.Append(declaration.Variable.Value);
                        else if (assignment.Left[i] is Identifier reference)
                            builder.Append(reference.Value);

                        builder.Append($"={temp}[{i}];");
                    }
                }
                else
                {
                    for (var i = 0; i < assignment.Left.Count; i++)
                    {
                        var left = assignment.Left[i];
                        if (left is VariableDeclaration declaration)
                        {
                            left = declaration.Variable;
                            Generate(declaration.Type, builder);
                        }

                        Generate(left, builder);
                        switch (assignment.Operator.Tag)
                        {
                            case OperatorTag.PlusEqual:
                                builder.Append("+=");
                                break;
                            case OperatorTag.MinusEqual:
                                builder.Append("-=");
                                break;
                            case OperatorTag.MultiplyEqual:
                                builder.Append("*=");
                                break;
                            case OperatorTag.DivideEqual:
                                builder.Append("/=");
                                break;
                            case OperatorTag.ModuloEqual:
                                builder.Append("%=");
                                break;
                            case OperatorTag.BitwiseAndEqual:
                                builder.Append("&=");
                                break;
                            case OperatorTag.BitwiseOrEqual:
                                builder.Append("|=");
                                break;
                            case OperatorTag.BitwiseXorEqual:
                                builder.Append("^=");
                                break;
                            case OperatorTag.LeftShiftEqual:
                                builder.Append("<<=");
                                break;
                            case OperatorTag.RightShiftEqual:
                                builder.Append(">>=");
                                break;
                            case OperatorTag.Equal:
                                builder.Append("=");
                                break;
                            default:
                                Report.Error($"Cannot translate {assignment.Operator.Tag} to D.");
                                break;
                        }

                        Generate(assignment.Right[i], builder);
                        builder.Append(';');
                    }
                }
            }
            else if (statement is ExpressionStatement e)
            {
                Generate(e.Expression, builder);
                builder.Append(';');
            }
            else if (statement is ReturnStatement r)
            {
                builder.Append("return ");
                if (r.Expressions.Count > 1)
                    builder.Append("tuple(");

                Generate(r.Expressions, builder);
                if (r.Expressions.Count > 1)
                    builder.Append(')');

                builder.Append(';');
            }
            else if (statement is AssertStatement assert)
            {
                builder.Append("assert(");
                Generate(assert.Assertion, builder);

                builder.Append(");");
            }
            else if (statement is WhileStatement w)
            {
                builder.Append('{');
                if (w.Assignment != null)
                    Generate(w.Assignment, builder);

                builder.Append("while (");
                Generate(w.Condition, builder);
                builder.Append(')');
                Generate(w.Body, builder);
                builder.Append('}');
            }
            else
                Report.Error($"Cannot translate {statement.GetType().Name} to D.");
        }

        private static void Generate(List<Expression> expressions, StringBuilder builder)
        {
            if (expressions.Count != 0)
            {
                for (var i = 0; i < expressions.Count; i++)
                {
                    Generate(expressions[i], builder);
                    if (i + 1 != expressions.Count)
                        builder.Append(',');
                }
            }
        }

        private static void Generate(Expression expression, StringBuilder builder)
        {
            if (expression is BinaryExpression binary)
            {
                Generate(binary.Left, builder);
                switch (binary.Operator.Tag)
                {
                    case OperatorTag.EqualEqual:
                        builder.Append("==");
                        break;
                    case OperatorTag.NotEqual:
                        builder.Append("!=");
                        break;
                    default:
                        Report.Error($"Cannot translate {binary.Operator.Tag} to D.");
                        break;
                }

                Generate(binary.Right, builder);
            }
            else if (expression is Call call)
            {
                Generate(call.Reference, builder);

                builder.Append('(');
                for (var i = 0; i < call.Arguments.Count; i++)
                {
                    Generate(call.Arguments[i], builder);
                    if (i + 1 != call.Arguments.Count)
                        builder.Append(',');
                }

                builder.Append(')');
            }
            else if (expression is Number number)
            {
                if (number.Value == "-9223372036854775808" && number.Tag == NumberTag.I64)
                    builder.Append("long.min"); // For some reason that constant overflows at compile time
                else                            // but long.min doesn't.
                {
                    builder.Append(number.Value);
                    switch (number.Tag)
                    {
                        case NumberTag.I64:
                            builder.Append("L");
                            break;
                        case NumberTag.U64:
                            builder.Append("uL");
                            break;
                        case NumberTag.U32:
                        case NumberTag.U16:
                        case NumberTag.U8:
                            builder.Append("u");
                            break;
                    }
                }
            }
            else if (expression is Identifier reference)
                builder.Append(reference.Value);
            else if (expression is Dereference dereference)
            {
                builder.Append('*');
                Generate(dereference.Expression, builder);
            }
            else if (expression is AddressOf addressOf)
            {
                builder.Append('&');
                Generate(addressOf.Expression, builder);
            }
            else if (expression is DotExpression dot)
            {
                Generate(dot.Structure, builder);
                builder.Append('.');
                Generate(dot.Field, builder);
            }
            else if (expression is CompoundLiteral literal)
            {
                builder.Append(literal.Structure.Value);
                builder.Append(' ');
                builder.Append('(');

                var fields = ((CompoundDeclaration)literal.Type).Fields;
                for (var i = 0; i < fields.Count; i++)
                {
                    var initializer = literal.Initializers.FirstOrDefault(f => f.Name.Value == fields[i].Name.Value);
                    if (initializer == null)
                        builder.Append('0');
                    else
                        Generate(initializer.Value, builder);

                    if (i + 1 != fields.Count)
                        builder.Append(',');
                }

                builder.Append(')');
            }
            else if (expression is Boolean boolean)
                builder.Append(boolean.Value);
            else if (expression is PostfixIncrement pfi)
            {
                Generate(pfi.Expression, builder);
                builder.Append("++");
            }
            else
                Report.Error($"{expression.Start} Cannot translate {expression.GetType().Name} to D.");
        }
    }
}