using System;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Text.RegularExpressions;

namespace Test
{
    internal static class Runner
    {
        private static void Main()
        {
            var exitCode = new Regex(@"^\s*\/\/\s*expect\s+(\d+)\s*$", RegexOptions.Compiled);
            var error = new Regex(@"^\s*\/\/\s*expect\s+(.*)$", RegexOptions.Compiled);
            var comment = new Regex(@"^\s*\/\/(.*)$", RegexOptions.Compiled);

            foreach (var path in Directory.EnumerateFiles(Environment.CurrentDirectory, "*.owen", SearchOption.AllDirectories))
            {
                var meta = default(string);
                var lines = File.ReadAllLines(path);

                for (var i = 0; i < lines.Length; i++)
                {
                    var match = exitCode.Match(lines[i]);
                    if (match.Success)
                        meta = match.Groups[1].Value;
                    else if ((match = error.Match(lines[i])).Success)
                    {
                        meta = match.Groups[1].Value.Trim();
                        if (i + 1 < lines.Length)
                        {
                            while (i + 1 < lines.Length && (match = comment.Match(lines[++i])).Success)
                                meta += $"\r\n{match.Groups[1].Value.Trim()}";

                            i--;
                        }
                    }
                    else
                        break;
                }

                var relativePath = path.Substring(Environment.CurrentDirectory.Length + 1);
                if (meta != null)
                {
                    var debug = Run(meta, relativePath, false);
                    var release = Run(meta, relativePath, true);

                    if (debug.Error || release.Error)
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine(relativePath);

                        Console.ForegroundColor = ConsoleColor.White;
                        
                        if (debug.Error)
                        {
                            Console.ForegroundColor = ConsoleColor.White;
                            Console.Write("Debug: ");
                            Console.ForegroundColor = ConsoleColor.Green;
                            Console.WriteLine(meta);

                            Console.ForegroundColor = ConsoleColor.Red;
                            Console.WriteLine($"       {debug.Message}");
                        }

                        if (release.Error)
                        {
                            Console.ForegroundColor = ConsoleColor.White;
                            Console.Write("Release: ");
                            Console.ForegroundColor = ConsoleColor.Green;
                            Console.WriteLine(meta);

                            Console.ForegroundColor = ConsoleColor.Red;
                            Console.WriteLine($"         {release.Message}");
                        }

                        Console.WriteLine();
                    }
                    else
                    {
                        Console.ForegroundColor = ConsoleColor.Green;
                        Console.WriteLine(relativePath);
                    }
                }
            }
            
            Console.ForegroundColor = ConsoleColor.White;
        }

        private static (bool Error, string Message) Run(string expect, string path, bool release)
        {
            if (File.Exists("program.exe"))
                File.Delete("program.exe");

            var compiler = new Process();
            compiler.StartInfo = new ProcessStartInfo("owen.exe", $"-input \"{Path.GetFullPath(path)}\" -output program.exe {(release ? "-release" : "")}")
            {
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true
            };

            compiler.Start();
            if (compiler.WaitForExit(5000))
            {
                if (compiler.ExitCode != 0)
                {
                    var error = compiler.StandardError.ReadToEnd().Trim();
                    if (string.IsNullOrWhiteSpace(error))
                        return (true, error);
                    else
                    {
                        if (error == expect)
                            return (false, null);
                        else
                            return (true, error);
                    }
                }
                else
                {
                    var program = new Process();
                    program.StartInfo = new ProcessStartInfo("program.exe")
                    {
                        RedirectStandardOutput = true,
                        RedirectStandardError = true,
                        UseShellExecute = false,
                        CreateNoWindow = true
                    };

                    try
                    {
                        program.Start();
                        if (program.WaitForExit(5000))
                        {
                            if (program.ExitCode.ToString() == expect)
                                return (false, null);
                            else
                                return (true, program.ExitCode.ToString());
                        }
                        else
                            return (true, "The program timed out.");
                    }
                    catch (Win32Exception ex)
                    {
                        return (true, ex.Message);
                    }
                }
            }
            else
            {
                compiler.Kill();
                return (true, "The compiler timed out.");
            }
        }
    }
}