using System;
using System.IO;
using System.Runtime.CompilerServices;

internal static class Report
{
    public static void Error(params string[] message)
    {
        throw new CompilationException(string.Join(Environment.NewLine, message));
    }

    public static void InternalError(string message, [CallerFilePath] string file = "", [CallerLineNumber] int line = 0)
    {
        throw new CompilationException($"INTERNAL COMPILER ERROR at {Path.GetFileName(file)}:{line}: {message}");
    }
}

internal sealed class CompilationException : Exception
{
    public CompilationException(string message) : base(message)
    {
    }
}