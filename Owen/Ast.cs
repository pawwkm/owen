namespace Owen
{
	internal class Program
    {
    }

	internal class File
	{
		public string Namespace;
	}

    internal class Source
    {
        public int Index;
        public string Text;
        public string Path;
    }

    internal class Identifier
    {
        public Position DeclaredAt;
        public string Value;
    }

    internal class Position
    {
        public string Path;
        public int Line;
        public int Column;

        public override string ToString() => 
            $"{Path}:{Line}:{Column}:";
    }
}