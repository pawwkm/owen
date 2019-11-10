using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using System.IO;

namespace Owen
{
    internal static class Compiler
    {
        private static void Main(string[] arguments)
        {
            var release = false;
            var checkOnly = false;
            var includePropositions = false;
            var output = "";
            var paths = (List<string>)null;

            for (var i = 0; i < arguments.Length; i++)
            {
				switch (arguments[i])
				{
					case "-release":
						release = true;
						break;
					case "-check-only":
						checkOnly = true;
						break;
					case "-propositions":
						includePropositions = true;
						break;
					case "-input":
						i++;

						paths = new List<string>();
						while (arguments.Length != i && !arguments[i].StartsWith("-"))
							paths.Add(arguments[i++]);

						i--;
						break;
					case "-output":
						if (arguments.Length == i + 1)
							Report.Error("Path expected.");
						else
							output = arguments[++i];
						break;
					case "-help":
					default:
						Report.Information("-input        The files to compile. If omitted then all files in the current directory are compiled.",
										   "-output       The path to the compiled program. This path must be specified.",
										   "-release      Compiles an optimized build without debugging information.",
										   "-check-only   Checks whether or not the program is valid without generating an executable.",
										   "-propositions Includes propositions in the build otherwise they are analyzed.");
						break;
				}
            }

            if (output == "" && !checkOnly)
                Report.Error("Specify where to put the executable.");
            else
            {
                if (paths == null)
                    paths = Directory.EnumerateFiles(Environment.CurrentDirectory, "*.owen", SearchOption.AllDirectories).ToList();

                var program = new Program();
                foreach (var path in paths)
                {
                    var mightBeRelativePath = path;
                    if (path.StartsWith(Environment.CurrentDirectory))
                        mightBeRelativePath = path.Substring(Environment.CurrentDirectory.Length + 1);

                    try
                    {
						Syntax.Parse(program, new Source()
						{
							Path = mightBeRelativePath,
							Text = System.IO.File.ReadAllText(path, Encoding.UTF8)
						});
                    }
                    catch (Exception)
                    {
                        Report.Error($"Could not read {mightBeRelativePath}");
                    }
                }

                Semantics.Analyzse(program);
                Ctfe.Execute(program);

                if (!checkOnly)
                {
                    //if (!includePropositions)
                        //program.Propositions.Clear();

                    if (release)
                        D.Generate(program, output);
                    else
                        IL.Generate(program, output);
                }
            }
        }
    }
}