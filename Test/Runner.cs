using System;
using System.Diagnostics;
using System.IO;

namespace Test
{
	internal static class Runner
	{
		private static void Main()
		{
			foreach (var path in Directory.EnumerateFiles(Environment.CurrentDirectory, "*.owen", SearchOption.AllDirectories))
			{
				var process = new Process();
				process.StartInfo = new ProcessStartInfo("owen.exe", $"-input \"{Path.GetFullPath(path)}\" -output program.exe")
				{
					RedirectStandardOutput = true,
					RedirectStandardError = true,
					UseShellExecute = false,
					CreateNoWindow = true
				};

				process.Start();
				if (process.WaitForExit(5000))
				{
					var response = process.StandardOutput.ReadToEnd();
					// Match the response with the expected result of the program.
					// Files without meta data are ignored. A file could specify 
					// other files to be included in the compilation. This is 
					// useful for testing namespaces.
				}
				else
				{
					process.Kill();

					var relativePath = path.Substring(Environment.CurrentDirectory.Length + 1);
					Console.WriteLine($"The compiler timed out with {relativePath}.");
				}
			}
		}
	}
}