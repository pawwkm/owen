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
                var builder = new StringBuilder();
                Generate(program.Files[i], builder);

                // dmd does not like file names starting with a digit.
                System.IO.File.WriteAllText
                (
                    Path.Combine(root, $"_{i}.d"), 
                    builder.ToString(),
                    Encoding.UTF8
                );
            }

            try
            {
                var compiler = new Process();
                compiler.StartInfo = new ProcessStartInfo("dmd.exe", $"-release -betterC -O -noboundscheck \"-of{output}\" {string.Join(" ", Enumerable.Range(0, program.Files.Count).Select(i => $"_{i}.d"))}")
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

        private static void Generate(File file, StringBuilder builder)
        {
            foreach (var function in file.Functions)
                Generate(function, builder);
        }

        private static void Generate(FunctionDeclaration function, StringBuilder builder)
        {
            if (function.Name.Value == "main")
                builder.Append("extern(C) ");

            GenerateType(function.Output, builder);
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

        private static void GenerateType(List<Identifier> types, StringBuilder builder)
        {
            if (types.Count == 0)
                builder.Append("void ");
            else if (types.Count == 1)
                GenerateType(types[0].Value, builder);
            else
                Report.Error("Cannot translate a tuple to D.");
        }

        private static void GenerateType(string type, StringBuilder builder)
        {
            switch (type)
            {
                case "I8":
                    builder.Append("byte ");
                    break;
                case "I16":
                    builder.Append("short ");
                    break;
                case "I32":
                    builder.Append("int ");
                    break;
                case "I64":
                    builder.Append("long ");
                    break;
                case "U8":
                    builder.Append("ubyte ");
                    break;
                case "U16":
                    builder.Append("ushort ");
                    break;
                case "U32":
                    builder.Append("uint ");
                    break;
                case "U64":
                    builder.Append("ulong ");
                    break;
                default:
                    Report.Error($"Cannot translate {type} to D.");
                    break;
            }
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
                }
            }
        }

        private static void Generate(CompoundStatement compound, StringBuilder builder)
        {
            builder.Append('{');
            foreach (var statement in compound.Statements)
            {
                if (statement is AssignmentStatement assignment)
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
                else if (statement is ReturnStatement r)
                {
                    builder.Append("return ");
                    Generate(r.Expressions, builder);

                    builder.Append(';');
                }
                else if (statement is AssertStatement assert)
                {
                    builder.Append("assert(");
                    Generate(assert.Assertion, builder);

                    builder.Append(");");
                }
                else
                    Report.Error($"Cannot translate {statement.GetType().Name} to D.");
            }

            builder.Append('}');
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
                }

                Generate(binary.Right, builder);
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
            else
                Report.Error($"Cannot translate {expression.GetType().Name} to D.");
        }
    }
}