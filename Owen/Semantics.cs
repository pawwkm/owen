using System.Linq;

namespace Owen
{
    internal static class Semantics
    {
        public static void Analyzse(Program program)
        {
            var mainFunctions = program.Files.SelectMany(f => f.Functions)
                                             .Where(f => f.Name.Value == "main")
                                             .ToList();

            if (mainFunctions.Count == 0)
                Report.Error("No main function defined.");
            else if (mainFunctions.Count > 1)
                Report.Error($"Multiple main functons defined:\r\n{string.Join("\r\n", mainFunctions.Select(f => f.Name.DeclaredAt))}");

            var main = mainFunctions[0];
            if (main.Output.Count != 1 || main.Output[0].Value != "i32")
                Report.Error($"{main.Name.DeclaredAt} main must output a single i32.");
        }
    }
}