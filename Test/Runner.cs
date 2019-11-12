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
            var exitCodePattern = new Regex(@"^\s*\/\/\s*expect\s+(\d+)\s*$", RegexOptions.Compiled);
            var errorPattern = new Regex(@"^\s*\/\/\s*expect\s+(.*)$", RegexOptions.Compiled);
            var commentPattern = new Regex(@"^\s*\/\/(.*)$", RegexOptions.Compiled);

            foreach (var path in Directory.EnumerateFiles(Environment.CurrentDirectory, "*.owen", SearchOption.AllDirectories))
            {
                var expect = default(string);
                var lines = File.ReadAllLines(path);

                for (var i = 0; i < lines.Length; i++)
                {
                    var match = exitCodePattern.Match(lines[i]);
                    if (match.Success)
                        expect = match.Groups[1].Value;
                    else if ((match = errorPattern.Match(lines[i])).Success)
                    {
                        expect = match.Groups[1].Value.Trim();
                        if (i + 1 < lines.Length)
                        {
                            while (i + 1 < lines.Length && (match = commentPattern.Match(lines[++i])).Success)
                                expect += $"\r\n{match.Groups[1].Value.Trim()}";

                            i--;
                        }
                    }
                    else
                        break;
                }

                var relativePath = path.Substring(Environment.CurrentDirectory.Length + 1);
                if (expect != null)
                {
                    var debug = Run(expect, relativePath, false);
                    var release = Run(expect, relativePath, true);

                    if (debug.Error || release.Error)
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine(relativePath);

                        Console.ForegroundColor = ConsoleColor.White;
                        
                        if (debug.Error)
                        {
                            Console.WriteLine($"Debug:");
                            Console.WriteLine(debug.Message);
                        }

                        if (release.Error)
                        {
                            Console.WriteLine($"Release:");
                            Console.WriteLine(release.Message);
                        }
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
                    var error = compiler.StandardError.ReadToEnd();
                    if (string.IsNullOrWhiteSpace(error))
                        return (true, error);
                    else
                    {
                        if (error == expect)
                            return (false, null);
                        else
                            return (false, $"Expected {expect} but got {error}.");
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
                                return (true, $"Expected {expect} but got {program.ExitCode}.");
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