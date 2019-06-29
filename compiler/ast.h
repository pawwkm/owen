#pragma once

#include <stdbool.h>
#include "list.h"

typedef struct
{
    char* path;
    char* contents;
    char* current;
} Source;

DECLARE_LIST(Source)

typedef struct
{
    char* path;
    char* start;
    int32_t length;
} Slice;

DECLARE_LIST(Slice)

struct Symbol;
typedef struct
{
    struct Symbol* elements;
    int32_t count;
    int32_t capacity;
} SymbolList;

struct FunctionDeclaration;

typedef struct Type
{
    enum
    {
        TYPE_I8,
        TYPE_I16,
        TYPE_I32,
        TYPE_I64,
        TYPE_U8,
        TYPE_U16,
        TYPE_U32,
        TYPE_U64,
        TYPE_F32,
        TYPE_F64,
        TYPE_BOOL,
        TYPE_FUNCTION,
        TYPE_ENUMERATION,
        TYPE_STRUCTURE,
        TYPE_UNION
    } tag;
    union
    {
        struct
        {
            struct FunctionDeclaration* declaration;
            SymbolList input;
            SymbolList output;
        } function;
    };
} Type;

typedef struct Symbol
{
    char* name;
    Type type;
} Symbol;

typedef struct Scope
{
    struct Scope* parent;
    SymbolList symbols;
} Scope;

typedef struct
{
    enum
    {
        NUMBER_I8,
        NUMBER_I16,
        NUMBER_I32,
        NUMBER_I64,
        NUMBER_U8,
        NUMBER_U16,
        NUMBER_U32,
        NUMBER_U64,
        NUMBER_F32,
        NUMBER_F64,
        NUMBER_FLOAT_TO_BE_INFERRED,
        NUMBER_INTEGER_TO_BE_INFERRED
    } tag;
    Slice value;
    Source position;
} Number;

typedef struct
{
    char* value;
    Source position;
} Identifier;
DECLARE_LIST(Identifier)

typedef struct Expression
{
    enum
    {
        EXPRESSION_NONE,
        EXPRESSION_CALL,
        EXPRESSION_NUMBER,
        EXPRESSION_IDENTIFIER,
        EXPRESSION_CTFE,
        EXPRESSION_VARIABLE_DECLARATION,
        EXPRESSION_VARIABLE_REFERENCE
    } tag;

    union
    {
        Number number;
        Identifier identifier;
        struct Expression* expression;
        struct
        {
            Identifier name;
        } call;
    };

    void* ctfeValue;
} Expression;

DECLARE_LIST(Expression)

typedef struct
{
    enum
    {
        STATEMENT_ASSIGNMENT,
        STATEMENT_EXPRESSION,
        STATEMENT_RETURN
    } tag;

    union
    {
        Expression value;
        struct
        {
            ExpressionList variables;
            ExpressionList values;
        } assignment;
    };
} Statement;

DECLARE_LIST(Statement)

typedef struct FunctionDeclaration
{
    bool isPublic;
    Identifier name;
    IdentifierList output;
    StatementList body;
    Scope scope;
    bool analyzed;
} FunctionDeclaration;

DECLARE_LIST(FunctionDeclaration)

typedef struct
{
    char* namespace;
    StringList uses;
    FunctionDeclarationList functions;
    Scope scope;
} File;

DECLARE_LIST(File)
typedef struct
{
    StringList interned;
    FileList files;
} Program;
