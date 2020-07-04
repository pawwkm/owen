using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.Emit;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

using static System.IO.File;

internal static class Compiler
{
    private static void Main(string[] arguments)
    {
        if (arguments.All(a => a != "-test"))
        {
            var includePropositions = false;
            var versions = new List<string>();

            for (var i = 0; i < arguments.Length;)
            {
                switch (arguments[i++])
                {
                    case "-propositions":
                        includePropositions = true;
                        break;
                    case "-versions":
                        while (i != arguments.Length && arguments[i][0] != '-')
                            versions.Add(arguments[i++]);
                        break;
                    case "-help":
                    default:
                        Console.WriteLine("usage: owen [options]");
                        Console.WriteLine();
                        Console.WriteLine("Compiles all *.owen files in the current directory and its subdirectories into ");
                        Console.WriteLine("a program with the same name as the source file that contains the main function");
                        Console.WriteLine("in the same directory as that file.");
                        Console.WriteLine();
                        Console.WriteLine("options:");
                        Console.WriteLine("    -versions     Adds the given version identifiers to the build.");
                        Console.WriteLine("    -propositions Includes propositions in the build.");
                        Console.WriteLine("    -test         Runs all compiler tests. Ignores other options.");
                        Console.WriteLine("    -help         Displays this message.");
                        return;
                }
            }

            var pathToContents = new Dictionary<string, string>();
            foreach (var path in Directory.EnumerateFiles(Environment.CurrentDirectory, "*.owen", SearchOption.AllDirectories))
            {
                var relativePath = path.Substring(Environment.CurrentDirectory.Length + 1);

                try
                {
                    pathToContents.Add(relativePath, ReadAllText(relativePath, Encoding.UTF8));
                }
                catch (PathTooLongException)
                {
                    Report.Error($"The path \"{relativePath}\" is to long.");
                }
                catch (UnauthorizedAccessException)
                {
                    Report.Error($"Could not access \"{relativePath}\".");
                }
                catch (IOException ex) when (ex is DirectoryNotFoundException || ex is FileNotFoundException)
                {
                    Report.Error($"Could not find \"{relativePath}\".");
                }
            }

            Compile(pathToContents, null, includePropositions, versions);
        }
        else
        {
            var tests = Path.Combine(Path.GetDirectoryName(Assembly.GetEntryAssembly().Location), "tests");

            // Delete previously compiled program as to not confuse them with what the current compiler 
            // generates. It would be very annoying inspecting an erroneous program only to find 
            // out that the current compiler no longer produces that code.
            foreach (var path in Directory.EnumerateFiles(tests, "*.*", SearchOption.AllDirectories).Where(p => p.EndsWith(".exe") || p.EndsWith(".pdb")))
                Delete(path);

            foreach (var path in Directory.EnumerateFiles(tests, "*.owen", SearchOption.AllDirectories).Where(p => !p.EndsWith(".include.owen")))
            {
                var keywords = new[] { "error", "file" };
                var meta = new Regex(@"^\s*(\/\/\s*(.*))?$");
                var lines = ReadAllLines(path).TakeWhile(l => meta.IsMatch(l))
                                              .Select(l => meta.Match(l))
                                              .Select(m => m.Groups[2].Value)
                                              .Where(l => !string.IsNullOrWhiteSpace(l))
                                              .ToList();

                var primaryFile = path.Substring(tests.Length + 1);
                var pathToContents = new Dictionary<string, string>();
                pathToContents.Add(primaryFile, ReadAllText(path, Encoding.UTF8));

                var compilationError = default(string);
                for (var i = 0; i < lines.Count; i++)
                {
                    if (lines[i].StartsWith("error"))
                    {
                        compilationError = RemoveFirstWord(lines[i]);
                        while (i + 1 < lines.Count && !keywords.Any(k => lines[i + 1].StartsWith(k)))
                            compilationError += $"{Environment.NewLine}{lines[++i]}";
                    }
                    else if (lines[i].StartsWith("file"))
                    {
                        var additionalPath = RemoveFirstWord(lines[i]);
                        pathToContents.Add(additionalPath, ReadAllText("tests\\" + additionalPath, Encoding.UTF8));
                    }
                }

                using (var writer = new StringWriter())
                {
                    WriteInWhite(primaryFile);
                    Console.WriteLine();

                    Console.SetError(writer);

                    try
                    {
                        var compiler = Task.Run(() => Compile(pathToContents, Path.ChangeExtension(path, "exe"), false, new List<string>()));
                        if (compiler.Wait(2000))
                        {
                            if (compiler.Result == null)
                            {
                                var output = writer.ToString().TrimEnd();
                                if (compilationError == null)
                                {
                                    WriteInWhite("    The compiler reported this unexpected error:");
                                    Console.WriteLine();

                                    WriteInRed(Indent(output));
                                    Console.WriteLine();
                                }
                                else if (compilationError != output)
                                {
                                    WriteInWhite("    Expected this compile error:");
                                    Console.WriteLine();

                                    WriteInGreen(Indent(compilationError));
                                    Console.WriteLine();

                                    WriteInWhite("    but was:");
                                    Console.WriteLine();

                                    WriteInRed(Indent(output));
                                    Console.WriteLine();
                                }
                            }
                            else
                            {
                                var program = Task.Run(() => compiler.Result());
                                if (program.Wait(2000))
                                {
                                    var output = writer.ToString().TrimEnd();
                                    if (output != "")
                                    {
                                        WriteInWhite("    The program had runtime errors:");
                                        Console.WriteLine();
                                        WriteInRed(Indent(writer.ToString()));
                                        Console.WriteLine();
                                    }
                                }
                                else
                                {
                                    WriteInRed("    The program timed out.");
                                    Console.WriteLine();
                                }
                            }
                        }
                        else
                        {
                            WriteInRed("    The compiler timed out.");
                            Console.WriteLine();
                        }
                    }
                    catch (AggregateException ex)
                    {
                        var inner = ex.InnerException;
                        while (inner.InnerException != null)
                            inner = inner.InnerException;

                        WriteInRed($"    {inner.Message}");
                        Console.WriteLine();

                        WriteInRed(Indent(inner.StackTrace));
                        Console.WriteLine();
                    }
                }
            }

            Console.ResetColor();
            void WriteInGreen(string text)
            {
                Console.ForegroundColor = ConsoleColor.Green;
                Console.Write(text);
            }

            void WriteInRed(string text)
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.Write(text);
            }

            void WriteInWhite(string text)
            {
                Console.ForegroundColor = ConsoleColor.White;
                Console.Write(text);
            }

            string RemoveFirstWord(string line) => Regex.Replace(line, @"^\s*\S*\s*", "");
            string Indent(string message) => string.Join("\n", message.Split('\n').Select(l => $"    {l.TrimEnd()}"));
        }
    }

    private static Action Compile(Dictionary<string, string> pathToContents, string output, bool includePropositions, List<string> versions)
    {
        try
        {
            var program = new Program();
            foreach (var (path, contents) in pathToContents)
                program.Files.Add(Syntax.Parse(path, contents, versions));

            Semantics.Analyze(program);
            if (output == null)
                output = Path.GetFullPath(Path.ChangeExtension(program.Files.First(a => a.Functions.Any(b => b.Name.Value == "main")).Path, "exe"));

            return IL.Generate(program, includePropositions, output, PEFileKinds.ConsoleApplication);
        }
        catch (CompilationException ex)
        {
            Console.Error.WriteLine(ex.Message);
            return null;
        }
    }

    // Call this method from Main to debug the compiler with a specific program.
    private static void Compile(string path)
    {
        Compile(new Dictionary<string, string>() { { path, ReadAllText(path) } }, null, false, new List<string>())?.Invoke();

        Environment.Exit(0);
    }
}