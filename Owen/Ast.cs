using System.Collections.Generic;

namespace Owen
{
	internal sealed class Program
    {
        public List<File> Files = new List<File>();
    }

	internal sealed class File
	{
		public string Namespace;
        public string Path;
        public List<FunctionDeclaration> Functions = new List<FunctionDeclaration>();
	}

    internal sealed class FunctionDeclaration
    {
        public Identifier Name;
        public List<Identifier> Output = new List<Identifier>();
        public CompoundStatement Body;
    }

    internal sealed class CompoundStatement
    {
        public List<Statement> Statements = new List<Statement>();
    }

    internal abstract class Statement
    {
    }

    internal sealed class ReturnStatement : Statement
    {
        public List<Expression> Expressions;
    }

    internal abstract class Expression
    {
    }

    internal sealed class Number : Expression
    {
        public Position DeclaredAt;
        public string Value;
        public NumberTag Tag;
    }
    
    public enum NumberTag
    {
        I8,
        I16,
        I32,
        I64,
        U8,
        U16,
        U32,
        U64,
        F32,
        F64,
        IntegerToBeInfered,
        FloatToBeInfered
    }

    internal sealed class Source
    {
        public int Index;
        public string Text;
        public Position Position;
        public bool EndOfText => Text.Length == Index;
    }

    internal sealed class Identifier
    {
        public Position DeclaredAt;
        public string Value;
    }

    internal sealed class Position
    {
        public string Path;
        public int Index;
        public int Line = 1;
        public int Column = 1;

        public Position Copy() => new Position()
        {
            Path = Path,
            Index = Index,
            Line = Line,
            Column = Column
        };

        public override string ToString() => 
            $"{Path}:{Line}:{Column}:";
    }
}