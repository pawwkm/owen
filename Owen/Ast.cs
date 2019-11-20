using System;
using System.Collections.Generic;

namespace Owen
{
	internal sealed class Program
    {
        public Scope Scope = new Scope(); 
        public List<File> Files = new List<File>();
    }

	internal sealed class File
	{
        public Scope Scope;
		public Identifier Namespace;
        public string Path;
        public List<FunctionDeclaration> Functions = new List<FunctionDeclaration>();
        public List<Expression> Ctfe = new List<Expression>();
        public List<Expression> Mixins = new List<Expression>();
	}

    internal sealed class FunctionDeclaration
    {
        public Identifier Name;
        public List<Argument> Input = new List<Argument>();
        public List<Identifier> Output = new List<Identifier>();
        public CompoundStatement Body;
    }

    internal sealed class Argument
    {
        public Type Type;
        public Identifier Name;
    }

    internal sealed class CompoundStatement
    {
        public Scope Scope = new Scope();
        public List<Statement> Statements = new List<Statement>();
    }

    internal sealed class Scope
    {
        public Scope Parent;
        public List<Symbol> Symbols = new List<Symbol>();
    }

    internal sealed class Symbol
    {
        public string Name;
        public Type Type;
    }

    internal abstract class Type
    {
    }

    internal sealed class PrimitiveType : Type
    {
        public PrimitiveTypeTag Tag;
        public override string ToString() =>
            Tag.ToString();
    }

    internal sealed class UnresolvedType : Type
    {
        public Identifier Identifier;
    }

    internal sealed class FunctionType : Type
    {
        public FunctionDeclaration Declaration;
        public List<Type> Input = new List<Type>();
        public List<Type> Output = new List<Type>();
    }

    internal enum PrimitiveTypeTag
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
        Bool
    }

    internal abstract class Statement
    {
    }

    internal sealed class AssignmentStatement : Statement
    {
        public List<Expression> Left;
        public Operator Operator;
        public List<Expression> Right;
    }

    internal sealed class Operator
    {
        public Position DefinedAt;
        public OperatorTag Tag;
    }

    internal enum OperatorTag
    {
        PlusEqual,
        MinusEqual,
        MultiplyEqual,
        DivideEqual,
        BitwiseAndEqual,
        BitwiseOrEqual,
        BitwiseXorEqual,
        ModuloEqual,
        LeftShiftEqual,
        RightShiftEqual,
        Equal,

        EqualEqual,
        NotEqual
    }

    internal sealed class VariableDeclaration : Expression
    {
        public Type Type;
        public Identifier Variable;
    }

    internal sealed class ReturnStatement : Statement
    {
        public Position EndOfKeyword;
        public List<Expression> Expressions;
    }

    internal sealed class AssertStatement : Statement
    {
        public Expression Assertion;
    }

    internal abstract class Expression
    {
        public Position StartsAt()
        {
            if (this is BinaryExpression binary)
                return binary.Left.StartsAt();
            else if (this is Number number)
                return number.DeclaredAt;
            else if (this is Identifier reference)
                return reference.DeclaredAt;
            else
                throw new NotImplementedException($"Cannot find position of {GetType().Name}.");
        }
    }

    internal sealed class BinaryExpression : Expression
    {
        public Expression Left;
        public Operator Operator;
        public Expression Right;
    }

    internal sealed class Number : Expression
    {
        public Position DeclaredAt;
        public string Value;
        public NumberTag Tag;
    }
    
    internal enum NumberTag
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

    internal sealed class Call : Expression
    {
        public FunctionDeclaration DeclarationOfCallee;
        public Expression Callee;
        public List<Expression> Arguments;
    }

    internal sealed class Source
    {
        public int Index;
        public string Text;
        public Position Position;
        public bool EndOfText => Text.Length == Index;
    }

    internal sealed class Identifier : Expression
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