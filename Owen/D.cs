using System;
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
                System.IO.File.WriteAllText(Path.Combine(root, $"_{i}.d"), builder.ToString());
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

            GenerateType(function.Output[0].Value, builder);
            builder.Append(function.Name.Value);
            builder.Append("()");

            Generate(function.Body, builder);
        }

        private static void GenerateType(string type, StringBuilder builder)
        {
            if (type == null)
                builder.Append("void ");
            else
            {
                switch (type)
                {
                    case "i8":
                        builder.Append("byte ");
                        break;
                    case "i16":
                        builder.Append("short ");
                        break;
                    case "i32":
                        builder.Append("int ");
                        break;
                    case "i64":
                        builder.Append("long ");
                        break;
                    case "u8":
                        builder.Append("ubyte ");
                        break;
                    case "u16":
                        builder.Append("ushort ");
                        break;
                    case "u32":
                        builder.Append("uint ");
                        break;
                    case "u64":
                        builder.Append("ulong ");
                        break;
                    default:
                        throw new NotImplementedException($"Cannot translate {type} to a D type.");
                }
            }
        }

        private static void Generate(CompoundStatement compound, StringBuilder builder)
        {
            builder.Append('{');
            foreach (var statement in compound.Statements)
            {
                if (statement is ReturnStatement r)
                    Generate(r, builder);
                else
                    throw new NotImplementedException($"Cannot translate {statement.GetType().Name} to a D statement.");
            }

            builder.Append('}');
        }

        private static void Generate(ReturnStatement statement, StringBuilder builder)
        {
            builder.Append("return ");
            Generate(statement.Expressions, builder);

            builder.Append(';');
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
            if (expression is Number number)
                Generate(number, builder);
            else
                throw new NotImplementedException($"Cannot translate {expression.GetType().Name} to a D expression.");
        }

        private static void Generate(Number number, StringBuilder builder)
        {
            builder.Append(number.Value);
        }
    }
}