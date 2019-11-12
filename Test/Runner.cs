using System;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;

namespace Test
{
    internal static class Runner
    {
        private static void Main()
        {
            var expectPattern = new Regex(@"^\s*\/\/\s*expect\s+(\d+)\s*$", RegexOptions.Compiled);
            foreach (var path in Directory.EnumerateFiles(Environment.CurrentDirectory, "*.owen", SearchOption.AllDirectories))
            {
                var expect = default(int?);
                foreach (var line in File.ReadAllLines(path))
                {
                    var expectMatch = expectPattern.Match(line);
                    if (expectMatch.Success)
                        expect = int.Parse(expectMatch.Groups[1].Value);
                    else
                        break;
                }

                var relativePath = path.Substring(Environment.CurrentDirectory.Length + 1);
                if (expect.HasValue)
                {
                    var debug = Run(expect.Value, relativePath, false);
                    var release = Run(expect.Value, relativePath, true);

                    if (debug.Error || release.Error)
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine(relativePath);

                        Console.ForegroundColor = ConsoleColor.White;
                        
                        if (debug.Error)
                            Console.WriteLine($"\tDebug:   {debug.Message}");
                        if (release.Error)
                            Console.WriteLine($"\tRelease: {release.Message}");
                    }
                    else
                    {
                        Console.ForegroundColor = ConsoleColor.Green;
                        Console.WriteLine(relativePath);
                    }
                }
            }
        }

        private static (bool Error, string Message) Run(int expect, string path, bool release)
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
                if (compiler.ExitCode == 0)
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
                            if (program.ExitCode == expect)
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
                else
                    return (true, string.Join(Environment.NewLine, compiler.StandardOutput
                                                                           .ReadToEnd()
                                                                           .Split()
                                                                           .Select(l => $"\t{l}")));
            }
            else
            {
                compiler.Kill();
                return (true, "The compiler timed out.");
            }
        }
    }
}