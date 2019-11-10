using System;

namespace Owen
{
    internal static class Report
    {
        public static void Error(params string[] message)
        {
            foreach (var line in message)
                Console.WriteLine(line);

            Environment.Exit(1);
        }

        public static void Information(params string[] message)
        {
            foreach (var line in message)
                Console.WriteLine(line);

            Environment.Exit(0);
        }
    }
}