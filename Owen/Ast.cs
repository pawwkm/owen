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
        public List<Identifier> Uses = new List<Identifier>();
        public List<string> PathsToRenferencedFiles = new List<string>();
        public string Path;
        public List<FunctionDeclaration> Functions = new List<FunctionDeclaration>();
        public List<CompoundStatement> Propositions = new List<CompoundStatement>();
        public List<EnumerationDeclaration> Enumerations = new List<EnumerationDeclaration>();
        public List<CompoundDeclaration> Compounds = new List<CompoundDeclaration>();
        public List<Expression> Ctfe = new List<Expression>();
        public List<Expression> Mixins = new List<Expression>();
	}

    internal sealed class FunctionDeclaration : Type
    {
        public bool IsPublic;
        public bool IsExternal;
        public String Library;
        public Identifier Name;
        public List<Identifier> Generalized = new List<Identifier>();
        public List<FunctionDeclaration> Resolved = new List<FunctionDeclaration>();
        public List<Argument> Input = new List<Argument>();
        public Type Output;
        public CompoundStatement Body;
    }

    internal sealed class Argument
    {
        public Type Type;
        public Identifier Name;
    }

    internal sealed class EnumerationDeclaration : Type
    {
        public bool IsPublic;
        public Identifier Name;
        public Type Type;

        public List<EnumerationConstant> Constants = new List<EnumerationConstant>();
        public override string ToString() => Name.Value;
    }

    internal sealed class EnumerationConstant
    {
        public Identifier Name;
        public Number Value;
    }

    internal sealed class CompoundStatement : Statement
    {
        public Scope Scope = new Scope();
        public List<Statement> Statements = new List<Statement>();
    }

    internal sealed class Scope
    {
        public Scope Parent;
        public List<Symbol> Symbols = new List<Symbol>();
    }

    internal class Symbol
    {
        public string Name;
        public Type Type;
    }

    internal sealed class LocalSymbol : Symbol
    {
        public ushort Index;
    }

    internal sealed class InputSymbol : Symbol
    {
        public ushort Index;
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

    internal sealed class TupleType : Type
    {
        public List<Type> Types;
    }

    internal sealed class Pointer : Type
    {
        public Type To;
        public override string ToString() => $"#{To}";
    }

    internal sealed class Array : Type
    {
        public Number Length;
        public Type Of;
        public override string ToString() => Length == null ? 
                                             $"[]{Of}" : 
                                             $"[{Length}]{Of}";
    }

    internal sealed class Null : Type
    {
        public override string ToString() => "null";
    }

    internal sealed class NullLiteral : Expression
    {
    }

    internal sealed class CompoundDeclaration : Type
    {
        public bool IsPublic;
        public CompoundTypeTag Tag;
        public Identifier Name;
        public List<Field> Fields = new List<Field>();

        public override string ToString() => Name.Value;
    }

    internal enum CompoundTypeTag
    {
        Structure,
        Union
    }

    internal sealed class Field
    {
        public Type Type;
        public Identifier Name;
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

    internal sealed class IfStatement : Statement
    {
        public List<ConditionalBlock> Blocks = new List<ConditionalBlock>();
    }

    internal sealed class ConditionalBlock
    {
        public AssignmentStatement Assignment;
        public Expression Condition;
        public CompoundStatement Body;
    }

    internal sealed class ForStatement : Statement
    {
        public AssignmentStatement Assignment;
        public Expression Condition;
        public AssignmentStatement Post;
        public CompoundStatement Body;
    }

    internal sealed class WhileStatement : Statement
    {
        public AssignmentStatement Assignment;
        public Expression Condition;
        public CompoundStatement Body;
    }

    internal sealed class Operator
    {
        public Position Start
        {
            get;
            set;
        }

        public Position End
        {
            get;
            set;
        }

        public OperatorTag Tag;
    }

    internal enum OperatorTag
    {
        LogicalOr,
        LogicalAnd,
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
        NotEqual,
        LessThanOrEqual,
        GreaterThanOrEqual,
        LessThan,
        GreaterThan,

        Plus,
        Minus,
        BitwiseOr,
        BitwiseXor,

        Multiply,
        Divide,
        Modulo,
        BitwiseAnd,
        LeftShift,
        RightShift
    }

    internal sealed class VariableDeclaration : Expression
    {
        public Identifier Variable;
    }

    internal sealed class ExpressionStatement : Statement
    {
        public Expression Expression;
    }

    internal sealed class ReturnStatement : Statement
    {
        public Position Start;
        public List<Expression> Expressions;
    }

    internal sealed class AssertStatement : Statement
    {
        public Expression Assertion;
    }

    internal sealed class BreakStatement : Statement
    {
        public Position Start;
    }

    internal abstract class Expression
    {
        public Type Type;

        public Position Start
        {
            get;
            set;
        }

        public Position End
        {
            get;
            set;
        }
    }

    internal sealed class BinaryExpression : Expression
    {
        public Expression Left;
        public Operator Operator;
        public Expression Right;
    }

    internal sealed class SizeOf : Expression
    {
        public Type TypeBeingSizedUp;
    }

    internal sealed class AddressOf : Expression
    {
        public Expression Expression;
    }

    internal sealed class Dereference : Expression
    {
        public Expression Expression;
    }

    internal sealed class DotExpression : Expression
    {
        public Expression Structure;
        public Expression Field;
    }

    internal sealed class CompoundLiteral : Expression
    {
        public Identifier Structure;
        public List<FieldInitializer> Initializers = new List<FieldInitializer>();
    }

    internal sealed class ArrayLiteral : Expression
    {
        public Type ElementType;
    }

    internal sealed class FieldInitializer
    {
        public Identifier Name;
        public Expression Value;
    }

    internal sealed class Number : Expression
    {
        public string Value;
        public NumberTag Tag;
    }

    internal sealed class Not : Expression
    {
        public Expression Expression;
    }

    internal sealed class Negate : Expression
    {
        public Expression Expression;
    }

    internal sealed class Boolean : Expression
    {
        public string Value;
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
        ToBeInfered
    }

    internal sealed class Call : Expression
    {
        public List<Type> Generics;
        public FunctionDeclaration Declaration;
        public Expression Reference;
        public List<Expression> Arguments;
    }

    internal sealed class Source
    {
        public int Index;
        public List<Token> Tokens;
        public Token Current => EndOfTokens ? null : Tokens[Index];
        public Position Position => Current?.Start;
        public bool EndOfTokens => Tokens.Count == Index;
    }

    internal sealed class Identifier : Expression
    {
        public string Value;
    }

    internal sealed class String : Expression
    {
        public string Value;
    }

    internal sealed class Cast : Expression
    {
        public Expression Expression;
        public Type To;
    }

    internal sealed class Index : Expression
    {
        public Expression Array;
        public Expression Position;
    }

    internal sealed class Token
    {
        public Position Start;
        public Position End;
        public string Value;
        public TokenTag Tag;
    }

    internal enum TokenTag
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
        NumberToBeInfered,
        Identifier,
        Namespace,
        Use,
        Public,
        External,
        Function,
        Input,
        Output,
        End,
        If,
        Else,
        For,
        While,
        Break,
        Structure,
        Proposition,
        Enumeration,
        Of,
        Size,
        Union,
        Return,
        Mixin,
        Ctfe,
        True,
        False,
        Assert,
        Null,
        Generalize,
        Version,
        Cast,
        String,
        LogicalOr,
        LogicalAnd,
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
        NotEqual,
        LessThanOrEqual,
        GreaterThanOrEqual,
        LessThan,
        GreaterThan,
        Plus,
        Minus,
        BitwiseOr,
        BitwiseXor,
        Multiply,
        Divide,
        Modulo,
        BitwiseAnd,
        LeftShift,
        RightShift,
        Not,
        AddressOf,
        Dereference,
        Dot,
        LeftParentheses,
        RightParentheses,
        LeftSquareBracket,
        RightSquareBracket,
        Comma,
        Semicolon,
        Colon
    }

    internal sealed class Position
    {
        public string Path;
        public int Line = 1;
        public int Column = 1;

        public Position Copy() => new Position()
        {
            Path = Path,
            Line = Line,
            Column = Column
        };

        public override string ToString() => 
            $"{Path}:{Line}:{Column}:";
    }
}