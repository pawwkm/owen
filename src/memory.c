#include "compiler.h"
#include <string.h>

#include <stdio.h>

Function_Handle main_function = { .index = UINT16_MAX };

char characters[CHARACTER_CAPACITY];
uint32_t characters_length;

Interned_String interned_strings[INTERNED_STRING_CAPACITY];
uint16_t interned_strings_length;
const Interned_String_Handle invalid_interned_string_handle = { .index = UINT16_MAX };

Ir_Function ir_functions[IR_FUNCTION_CAPACITY];
uint16_t ir_functions_length; 
const Ir_Function_Handle invalid_ir_function_handle = { .index = UINT16_MAX };
Ir_Function_Handle ir_function_handles[]; 
uint16_t ir_function_handles_length;

uint8_t  text_section[TEXT_SECTION_CAPACITY];
uint32_t text_section_length;

#define DEFAULT_ADD_IMPLEMENTATION(TYPE, LOWER_CASE_TYPE, LOWER_CASE_TYPE_PLURAL, UPPER_CASE_TYPE) \
TYPE##_Handle add_##LOWER_CASE_TYPE(void)                                                          \
{                                                                                                  \
    if (LOWER_CASE_TYPE_PLURAL##_length == UPPER_CASE_TYPE##_CAPACITY)                             \
        out_of_memory(__FILE__, __LINE__, #TYPE);                                                  \
                                                                                                   \
    return (TYPE##_Handle) { .index = LOWER_CASE_TYPE_PLURAL##_length++ };                         \
}

#define DEFAULT_COMPARE_IMPLEMENTION(TYPE, LOWER_CASE_TYPE, LOWER_CASE_TYPE_PLURAL)      \
bool compare_##LOWER_CASE_TYPE_PLURAL(TYPE##_Handle a, TYPE##_Handle b)                  \
{                                                                                        \
    return a.index == b.index;                                                           \
}                                                                                        \
                                                                                         \
bool is_invalid_##LOWER_CASE_TYPE##_handle(TYPE##_Handle handle)                         \
{                                                                                        \
    return compare_##LOWER_CASE_TYPE_PLURAL(handle, invalid_##LOWER_CASE_TYPE##_handle); \
}

#define DEFAULT_LOOKUP_IMPLEMENTION(TYPE, LOWER_CASE_TYPE, LOWER_CASE_TYPE_PLURAL, UPPER_CASE_TYPE) \
TYPE* lookup_##LOWER_CASE_TYPE(TYPE##_Handle handle)                                                \
{                                                                                                   \
    assert(handle.index != invalid_##LOWER_CASE_TYPE##_handle.index);                               \
    assert(handle.index < UPPER_CASE_TYPE##_CAPACITY);                                              \
    assert(handle.index < LOWER_CASE_TYPE_PLURAL##_length);                                         \
                                                                                                    \
    return &LOWER_CASE_TYPE_PLURAL[handle.index];                                                   \
}

#define DEFAULT_HANDLE_ARRAY_IMPLEMENTATION(TYPE, LOWER_CASE_TYPE, UPPER_CASE_TYPE)                                    \
void reserve_##LOWER_CASE_TYPE##_handles(TYPE##_Handle_Array* array, uint8_t needed_free_handles)                      \
{                                                                                                                      \
    if (needed_free_handles <= array->handles_capacity - array->handles_length)                                        \
        return;                                                                                                        \
                                                                                                                       \
    if (!array->handles)                                                                                               \
    {                                                                                                                  \
        if (LOWER_CASE_TYPE##_handles_length + needed_free_handles > UPPER_CASE_TYPE##_CAPACITY)                       \
            out_of_memory(__FILE__, __LINE__, #TYPE);                                                                  \
                                                                                                                       \
        array->handles = &LOWER_CASE_TYPE##_handles[LOWER_CASE_TYPE##_handles_length];                                 \
        array->handles_capacity = needed_free_handles;                                                                 \
                                                                                                                       \
        LOWER_CASE_TYPE##_handles_length += needed_free_handles;                                                       \
    }                                                                                                                  \
    else if (array->handles == &LOWER_CASE_TYPE##_handles[LOWER_CASE_TYPE##_handles_length - array->handles_capacity]) \
    {                                                                                                                  \
        uint8_t left_to_reserve = needed_free_handles - (array->handles_capacity - array->handles_length);             \
        if (LOWER_CASE_TYPE##_handles_length + left_to_reserve > UPPER_CASE_TYPE##_CAPACITY)                           \
            out_of_memory(__FILE__, __LINE__, #TYPE);                                                                  \
                                                                                                                       \
        array->handles_capacity += left_to_reserve;                                                                    \
        LOWER_CASE_TYPE##_handles_length += left_to_reserve;                                                           \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        uint8_t left_to_reserve = needed_free_handles - (array->handles_capacity - array->handles_length);             \
        if (LOWER_CASE_TYPE##_handles_length + array->handles_capacity + left_to_reserve > UPPER_CASE_TYPE##_CAPACITY) \
            out_of_memory(__FILE__, __LINE__, #TYPE);                                                                  \
                                                                                                                       \
        TYPE##_Handle* destination = &LOWER_CASE_TYPE##_handles[LOWER_CASE_TYPE##_handles_length];                     \
        memcpy(destination, array->handles, sizeof(TYPE##_Handle) * array->handles_length);                            \
                                                                                                                       \
        array->handles = destination;                                                                                  \
        array->handles_capacity += left_to_reserve;                                                                    \
        LOWER_CASE_TYPE##_handles_length += array->handles_capacity;                                                   \
    }                                                                                                                  \
}                                                                                                                      \
                                                                                                                       \
void add_to_##LOWER_CASE_TYPE##_array(TYPE##_Handle_Array* array, TYPE##_Handle handle)                                \
{                                                                                                                      \
    reserve_##LOWER_CASE_TYPE##_handles(array, 1);                                                                     \
    array->handles[array->handles_length++] = handle;                                                                  \
}                                                                                                                      \
                                                                                                                       \
void insert_##LOWER_CASE_TYPE##_in_array(TYPE##_Handle_Array* array, TYPE##_Handle handle, uint8_t index)              \
{                                                                                                                      \
    if (array->handles_length == index)                                                                                \
        add_to_##LOWER_CASE_TYPE##_array(array, handle);                                                               \
    else if (array->handles_length <= index)                                                                           \
        print_error("%s:%u: ICE: Insertion out of bounds of " #TYPE "_Handle_Array.", __FILE__, __LINE__);             \
    else                                                                                                               \
    {                                                                                                                  \
        reserve_##LOWER_CASE_TYPE##_handles(array, 1);                                                                 \
        for (uint8_t i = array->handles_length - 1; i != index; i--)                                                   \
            array->handles[i] = array->handles[i - 1];                                                                 \
                                                                                                                       \
        array->handles[index] = handle;                                                                                \
        array->handles_length++;                                                                                       \
    }                                                                                                                  \
}                                                                                                                      \
                                                                                                                       \
void remove_##LOWER_CASE_TYPE##_from_array(TYPE##_Handle_Array* array, uint8_t index)                                  \
{                                                                                                                      \
    if (array->handles_length <= index)                                                                                \
        print_error("%s:%u: ICE: Insertion out of bounds of " #TYPE "_Handle_Array.", __FILE__, __LINE__);             \
                                                                                                                       \
    for (uint8_t i = index; i < array->handles_length - 1; i++)                                                        \
        array->handles[i] = array->handles[i + 1];                                                                     \
                                                                                                                       \
    array->handles_length--;                                                                                           \
}                                                                                                                      \
                                                                                                                       \
TYPE##_Handle LOWER_CASE_TYPE##_handle_at(const TYPE##_Handle_Array* array, Array_Size index)                          \
{                                                                                                                      \
    if (array->handles_length < index)                                                                                 \
        print_error("%s:%u: ICE: Indexing out of bounds of " #TYPE "_Handle_Array.", __FILE__, __LINE__);              \
                                                                                                                       \
    return array->handles[index];                                                                                      \
}                                                                                                                      \
                                                                                                                       \
void replace_##LOWER_CASE_TYPE(TYPE##_Handle_Array* array, TYPE##_Handle handle, Array_Size index)                     \
{                                                                                                                      \
    if (array->handles_length < index)                                                                                 \
        print_error("%s:%u: ICE: Indexing out of bounds of " #TYPE "_Handle_Array.", __FILE__, __LINE__);              \
                                                                                                                       \
    array->handles[index] = handle;                                                                                    \
}                                                                                                                      \
                                                                                                                       \
TYPE* lookup_##LOWER_CASE_TYPE##_in(const TYPE##_Handle_Array* array, Array_Size index)                                \
{                                                                                                                      \
    return lookup_##LOWER_CASE_TYPE(LOWER_CASE_TYPE##_handle_at(array, index));                                        \
}                                                                                                                      \

#define DEFINE_ARENA(TYPE, LOWER_CASE_TYPE, LOWER_CASE_TYPE_PLURAL, UPPER_CASE_TYPE, UINT_X, UINT_X_MAX) \
TYPE LOWER_CASE_TYPE_PLURAL[UPPER_CASE_TYPE##_CAPACITY];                                                 \
UINT_X LOWER_CASE_TYPE_PLURAL##_length;                                                                  \
                                                                                                         \
const TYPE##_Handle invalid_##LOWER_CASE_TYPE##_handle = { .index = UINT_X_MAX };                        \
TYPE##_Handle LOWER_CASE_TYPE##_handles[UPPER_CASE_TYPE##_HANDLE_CAPACITY];                              \
UINT_X LOWER_CASE_TYPE##_handles_length;                                                                 \
                                                                                                         \
DEFAULT_ADD_IMPLEMENTATION(TYPE, LOWER_CASE_TYPE, LOWER_CASE_TYPE_PLURAL, UPPER_CASE_TYPE)               \
DEFAULT_COMPARE_IMPLEMENTION(TYPE, LOWER_CASE_TYPE, LOWER_CASE_TYPE_PLURAL)                              \
DEFAULT_LOOKUP_IMPLEMENTION(TYPE, LOWER_CASE_TYPE, LOWER_CASE_TYPE_PLURAL, UPPER_CASE_TYPE)              \
DEFAULT_HANDLE_ARRAY_IMPLEMENTATION(TYPE, LOWER_CASE_TYPE, UPPER_CASE_TYPE)

DEFINE_ARENA(File,                file,                files,                FILE,                uint8_t,  UINT8_MAX )
DEFINE_ARENA(Type,                type,                types,                TYPE,                uint16_t, UINT16_MAX)
DEFINE_ARENA(Type_Reference,      type_reference,      type_references,      TYPE_REFERENCE,      uint16_t, UINT16_MAX)
DEFINE_ARENA(Namespace,           namespace,           namespaces,           NAMESPACE,           uint16_t, UINT16_MAX)
DEFINE_ARENA(Function,            function,            functions,            FUNCTION,            uint16_t, UINT16_MAX)
DEFINE_ARENA(Formal_Parameter,    formal_parameter,    formal_parameters,    FORMAL_PARAMETER,    uint16_t, UINT16_MAX)
DEFINE_ARENA(Statement,           statement,           statements,           STATEMENT,           uint32_t, UINT32_MAX)
DEFINE_ARENA(Branch,              branch,              branches,             BRANCH,              uint16_t, UINT16_MAX)
DEFINE_ARENA(Expression,          expression,          expressions,          EXPRESSION,          uint32_t, UINT32_MAX)
DEFINE_ARENA(Field,               field,               fields,               FIELD,               uint16_t, UINT16_MAX)
DEFINE_ARENA(Field_Initializer,   field_initializer,   field_initializers,   FIELD_INITIALIZER,   uint16_t, UINT16_MAX)
DEFINE_ARENA(Constant,            constant,            constants,            CONSTANT,            uint16_t, UINT16_MAX)
DEFINE_ARENA(Element_Initializer, element_initializer, element_initializers, ELEMENT_INITIALIZER, uint16_t, UINT16_MAX)
DEFINE_ARENA(Variable,            variable,            variables,            VARIABLE,            uint16_t, UINT16_MAX)

DEFINE_ARENA(Ir_Basic_Block,      ir_basic_block,      ir_basic_blocks,      IR_BASIC_BLOCK,      uint16_t, UINT16_MAX)
DEFINE_ARENA(Ir_Instruction,      ir_instruction,      ir_instructions,      IR_INSTRUCTION,      uint32_t, UINT32_MAX)
DEFINE_ARENA(Ir_Operand,          ir_operand,          ir_operands,          IR_OPERAND,          uint16_t, UINT16_MAX)

DEFAULT_ADD_IMPLEMENTATION(Ir_Function,   ir_function, ir_functions, IR_FUNCTION)
DEFAULT_COMPARE_IMPLEMENTION(Ir_Function, ir_function, ir_functions             )
DEFAULT_LOOKUP_IMPLEMENTION(Ir_Function,  ir_function, ir_functions, IR_FUNCTION)

DEFAULT_COMPARE_IMPLEMENTION(Interned_String, interned_string, interned_strings);

#define SYMBOL_TABLE_POOL_CAPACITY (POLYMORPHIC_STACK_CAPACITY + 1)
Symbol_Table symbol_tables[SYMBOL_TABLE_POOL_CAPACITY];
uint8_t symbol_tables_length;
uint8_t deepest_symbol_table;
Symbol_Table* current_symbol_table;

void acquire_symbol_table()
{
    if (symbol_tables_length == SYMBOL_TABLE_POOL_CAPACITY)
        out_of_memory(__FILE__, __LINE__, "Symbol_Table");

    current_symbol_table = &symbol_tables[symbol_tables_length++];
    if (symbol_tables_length > deepest_symbol_table)
        deepest_symbol_table = symbol_tables_length;
}

void release_symbol_table(void)
{
    if (symbol_tables_length == 0)
        print_error("Trying to release the 0 symbol table.");

    *current_symbol_table = (Symbol_Table){ 0 };
    if (--symbol_tables_length)
        current_symbol_table = &symbol_tables[symbol_tables_length - 1];
    else
        current_symbol_table = NULL;
}

void add_symbol(Interned_String_Handle name, Span name_span, Type_Handle type)
{
    if (current_symbol_table->symbols_length == SYMBOL_TABLE_POOL_CAPACITY)
        out_of_memory(__FILE__, __LINE__, "Symbol");

    current_symbol_table->symbols[current_symbol_table->symbols_length++] = (Symbol)
    {
        .name = name,
        .name_span = name_span,
        .type = type
    };
}

void add_definition(Ir_Basic_Block* block, Ir_Definition definition)
{
    if (block->definitions_length == SYMBOL_CAPACITY)
        out_of_memory(__FILE__, __LINE__, "Ir_Definition");

    block->definitions[block->definitions_length++] = definition;
}

Interned_String_Handle add_interned_string(String string)
{
    // Based on https://craftinginterpreters.com/hash-tables.html

    // FNV-1a hash.
    uint32_t hash = 2166136261u;
    for (uint32_t i = 0; i < string.length; i++)
    {
        hash ^= (uint8_t)string.text[i];
        hash *= 16777619;
    }

    uint32_t index = hash % INTERNED_STRING_CAPACITY;
    uint32_t original = index;
    while (true)
    {
        Interned_String* interned = &interned_strings[index];
        if (interned->text == NULL)
        {
            interned->text = string.text;
            interned->length = string.length;

            interned_strings_length++;

            break;
        }
        else if (interned->length == string.length && !strncmp(string.text, interned->text, string.length))
            break;

        // Linear probe.
        index = (index + 1) % INTERNED_STRING_CAPACITY;
        if (index == original)
            out_of_memory(__FILE__, __LINE__, "Interned_String");
    }
    
    return (Interned_String_Handle) { .index = (uint16_t)index };
}

Interned_String* lookup_interned_string(Interned_String_Handle handle)
{
    assert(handle.index != invalid_interned_string_handle.index);
    assert(handle.index < INTERNED_STRING_CAPACITY);

    Interned_String* interned = &interned_strings[handle.index];
    assert(interned->text);

    return interned;
}

bool compare_strings(String a, String b)
{
    return a.length == b.length && !strncmp(a.text, b.text, a.length);
}

Polymorphic_Type_Mapping polymorphic_type_mappings[POLYMORPHIC_STACK_CAPACITY];
uint8_t polymorphic_type_mappings_length;
uint8_t deepest_polymorphic_type_mapping;
Polymorphic_Type_Mapping* current_polymorphic_type_mapping;

void acquire_polymorphic_type_mapping(void)
{
    if (polymorphic_type_mappings_length == POLYMORPHIC_STACK_CAPACITY)
        print_error("Capacity exceeded.");

    current_polymorphic_type_mapping = &polymorphic_type_mappings[polymorphic_type_mappings_length++];
    current_polymorphic_type_mapping->file = NULL;
    current_polymorphic_type_mapping->length = 0;
    current_polymorphic_type_mapping->monomorphisized_function= NULL;
    current_polymorphic_type_mapping->monomorphisized_compound = NULL;

    for (uint8_t i = 0; i < POLYMORPHIC_STACK_TYPES_CAPACITY; i++)
    {
        current_polymorphic_type_mapping->polymorphic_types[i] = invalid_type_reference_handle;
        current_polymorphic_type_mapping->monomorphic_types[i] = invalid_type_handle;
    }

    if (polymorphic_type_mappings_length > deepest_polymorphic_type_mapping)
        deepest_polymorphic_type_mapping = polymorphic_type_mappings_length;
}

void release_polymorphic_type_mapping(void)
{
    if (polymorphic_type_mappings_length == 0)
        print_error("Trying to release the 0'th polymorphic type mapping.");

    if (--polymorphic_type_mappings_length)
        current_polymorphic_type_mapping = &polymorphic_type_mappings[polymorphic_type_mappings_length - 1];
    else
        current_polymorphic_type_mapping = NULL;
}

static void repeat_character(char c, size_t amount)
{
    for (size_t i = 0; i < amount; i++)
        putchar(c);
}

static uint8_t digits_in_u32(uint32_t n)
{
    uint8_t digits = 1;
    while (n /= 10)
        digits++;

    return digits;
}

// 000.00XB
#define SI_FORMAT_LENGTH 8
const char* bytes_to_si(uint32_t bytes)
{
    static char* units[] = { "B ", "kB", "MB", "GB" };
    static char buffer[SI_FORMAT_LENGTH + 1];
    uint8_t unit_index = 0;

    float size = (float)bytes;
    while (size > 999)
    {
        size /= 1000;
        unit_index++;
    }
    
    int a = sprintf(buffer, "%.2f", size);
    while (buffer[a - 1] == '0')
        a--;

    if (buffer[a - 1] == '.')
        a--;

    sprintf(&buffer[a], units[unit_index]);

    return buffer;
}

// https://forbrains.co.uk/standards/si_and_data_size_units
//                                                      000.00XB     000.00XB
// Strings                        Capacity    Used      Backed by    sizeof(T)
// characters                     4000000     512686    4MB          1B
// interned_strings               16000       4014      256kB        16B
// 
// Arenas                         Capacity    Used      Backed by    sizeof(T)
// files                          100         1         9.6kB        96B
// types                          4000        13        256kB        64B
// type_references                20000       8000      960kB        48B
// namespaces                     400         1         4kB          10B
// functions                      8000        4000      768kB        96B
// formal_parameters              20000       4000      240kB        12B
// statements                     100000      12000     5.6MB        56B
// branches                       8000        8000      192kB        24B
// expressions                    300000      60000     12MB         40B
// fields                         2000        0         28kB         14B
// field_initializers             2000        0         32kB         16B
// constants                      2000        0         48kB         24B
// element_initializers           2000        0         24kB         12B
// variables                      2000        0         28kB         14B
// 
// Handles                        Capacity    Used      Backed by    sizeof(T)
// type_handles                   16000       2         32kB         2B
// type_reference_handles         20000       4000      40kB         2B
// namespace_handles              400         0         800B         2B
// function_handles               8000        4160      16kB         2B
// formal_parameter_handles       24000       4000      48kB         2B
// statement_handles              200000      12000     800kB        4B
// branch_handles                 10000       8000      20kB         2B
// expression_handles             100000      16000     400kB        4B
// field_handles                  2000        0         4kB          2B
// field_initializer_handles      2000        0         4kB          2B
// constant_handles               2000        0         4kB          2B
// element_initializer_handles    2000        0         4kB          2B
// variable_handles               2000        0         4kB          2B
// 
// Stacks                         Capacity    Used      Backed by    sizeof(T)
// symbol_tables                  9           1         4.05kB       450B
// polymorphic_type_mappings      8           0         576B         72B
// 
// Total                                                25.83MB
// kB = 1000B
// MB = 1000kB
// GB = 1000MB

void print_memory(void)
{
    struct
    {
        char* group;
        char* name;
        uint32_t capacity;
        uint32_t used;
        uint32_t backed_by;
        uint32_t size_of_t;
    } rows[] =
    {
        { "Strings",  "characters",                  CHARACTER_CAPACITY,                  characters_length,                  sizeof(characters),                  sizeof(characters[0])                  },
        { "Strings",  "interned_strings",            INTERNED_STRING_CAPACITY,            interned_strings_length,            sizeof(interned_strings),            sizeof(interned_strings[0])            },

        { "Arenas",   "files",                       FILE_CAPACITY,                       files_length,                       sizeof(files),                       sizeof(files[0])                       },
        { "Arenas",   "types",                       TYPE_CAPACITY,                       types_length,                       sizeof(types),                       sizeof(types[0])                       },
        { "Arenas",   "type_references",             TYPE_REFERENCE_CAPACITY,             type_references_length,             sizeof(type_references),             sizeof(type_references[0])             },
        { "Arenas",   "namespaces",                  NAMESPACE_CAPACITY,                  namespaces_length,                  sizeof(namespaces),                  sizeof(namespaces[0])                  },
        { "Arenas",   "functions",                   FUNCTION_CAPACITY,                   functions_length,                   sizeof(functions),                   sizeof(functions[0])                   },
        { "Arenas",   "formal_parameters",           FORMAL_PARAMETER_CAPACITY,           formal_parameters_length,           sizeof(formal_parameters),           sizeof(formal_parameters[0])           },
        { "Arenas",   "statements",                  STATEMENT_CAPACITY,                  statements_length,                  sizeof(statements),                  sizeof(statements[0])                  },
        { "Arenas",   "branches",                    BRANCH_CAPACITY,                     branches_length,                    sizeof(branches),                    sizeof(branches[0])                    },
        { "Arenas",   "expressions",                 EXPRESSION_CAPACITY,                 expressions_length,                 sizeof(expressions),                 sizeof(expressions[0])                 },
        { "Arenas",   "fields",                      FIELD_CAPACITY,                      fields_length,                      sizeof(fields),                      sizeof(fields[0])                      },
        { "Arenas",   "field_initializers",          FIELD_INITIALIZER_CAPACITY,          field_initializers_length,          sizeof(field_initializers),          sizeof(field_initializers[0])          },
        { "Arenas",   "constants",                   CONSTANT_CAPACITY,                   constants_length,                   sizeof(constants),                   sizeof(constants[0])                   },
        { "Arenas",   "element_initializers",        ELEMENT_INITIALIZER_CAPACITY,        element_initializers_length,        sizeof(element_initializers),        sizeof(element_initializers[0])        },
        { "Arenas",   "variables",                   VARIABLE_CAPACITY,                   variables_length,                   sizeof(variables),                   sizeof(variables[0])                   },
        { "Arenas",   "ir_functions",                IR_FUNCTION_CAPACITY,                ir_functions_length,                sizeof(ir_functions),                sizeof(ir_functions[0])                },
        { "Arenas",   "ir_basic_blocks",             IR_BASIC_BLOCK_CAPACITY,             ir_basic_blocks_length,             sizeof(ir_basic_blocks),             sizeof(ir_basic_blocks[0])             },
        { "Arenas",   "ir_instructions",             IR_INSTRUCTION_CAPACITY,             ir_instructions_length,             sizeof(ir_instructions),             sizeof(ir_instructions[0])             },
        { "Arenas",   "ir_operands",                 IR_OPERAND_CAPACITY,                 ir_operands_length,                 sizeof(ir_operands),                 sizeof(ir_operands[0])                 },

        { "Handles",  "type_handles",                TYPE_HANDLE_CAPACITY,                type_handles_length,                sizeof(type_handles),                sizeof(type_handles[0])                },
        { "Handles",  "type_reference_handles",      TYPE_REFERENCE_HANDLE_CAPACITY,      type_reference_handles_length,      sizeof(type_reference_handles),      sizeof(type_reference_handles[0])      },
        { "Handles",  "namespace_handles",           NAMESPACE_HANDLE_CAPACITY,           namespace_handles_length,           sizeof(namespace_handles),           sizeof(namespace_handles[0])           },
        { "Handles",  "function_handles",            FUNCTION_HANDLE_CAPACITY,            function_handles_length,            sizeof(function_handles),            sizeof(function_handles[0])            },
        { "Handles",  "formal_parameter_handles",    FORMAL_PARAMETER_HANDLE_CAPACITY,    formal_parameter_handles_length,    sizeof(formal_parameter_handles),    sizeof(formal_parameter_handles[0])    },
        { "Handles",  "statement_handles",           STATEMENT_HANDLE_CAPACITY,           statement_handles_length,           sizeof(statement_handles),           sizeof(statement_handles[0])           },
        { "Handles",  "branch_handles",              BRANCH_HANDLE_CAPACITY,              branch_handles_length,              sizeof(branch_handles),              sizeof(branch_handles[0])              },
        { "Handles",  "expression_handles",          EXPRESSION_HANDLE_CAPACITY,          expression_handles_length,          sizeof(expression_handles),          sizeof(expression_handles[0])          },
        { "Handles",  "field_handles",               FIELD_HANDLE_CAPACITY,               field_handles_length,               sizeof(field_handles),               sizeof(field_handles[0])               },
        { "Handles",  "field_initializer_handles",   FIELD_INITIALIZER_HANDLE_CAPACITY,   field_initializer_handles_length,   sizeof(field_initializer_handles),   sizeof(field_initializer_handles[0])   },
        { "Handles",  "constant_handles",            CONSTANT_HANDLE_CAPACITY,            constant_handles_length,            sizeof(constant_handles),            sizeof(constant_handles[0])            },
        { "Handles",  "element_initializer_handles", ELEMENT_INITIALIZER_HANDLE_CAPACITY, element_initializer_handles_length, sizeof(element_initializer_handles), sizeof(element_initializer_handles[0]) },
        { "Handles",  "variable_handles",            VARIABLE_HANDLE_CAPACITY,            variable_handles_length,            sizeof(variable_handles),            sizeof(variable_handles[0])            },
        { "Handles",  "ir_basic_block_handles",      IR_BASIC_BLOCK_HANDLE_CAPACITY,      ir_basic_block_handles_length,      sizeof(ir_basic_block_handles),      sizeof(ir_basic_block_handles[0])      },
        { "Handles",  "ir_instruction_handles",      IR_INSTRUCTION_HANDLE_CAPACITY,      ir_instruction_handles_length,      sizeof(ir_instruction_handles),      sizeof(ir_instruction_handles[0])      },
        { "Handles",  "ir_operand_handles",          IR_OPERAND_HANDLE_CAPACITY,          ir_operand_handles_length,          sizeof(ir_operand_handles),          sizeof(ir_operand_handles[0])          },

        { "Stacks",   "symbol_tables",               SYMBOL_TABLE_POOL_CAPACITY,          deepest_symbol_table,               sizeof(symbol_tables),               sizeof(symbol_tables[0])               },
        { "Stacks",   "polymorphic_type_mappings",   POLYMORPHIC_STACK_CAPACITY,          deepest_polymorphic_type_mapping,   sizeof(polymorphic_type_mappings),   sizeof(polymorphic_type_mappings[0])   },

        { "Sections", "text_section",                TEXT_SECTION_CAPACITY,               text_section_length,                sizeof(text_section),                sizeof(text_section[0])                }
    };

    uint8_t longest_name_length      = 0;
    uint8_t longest_capacity_length  = (uint8_t)strlen("Capacity");
    uint8_t longest_used_length      = (uint8_t)strlen("Used");
    uint8_t longest_backed_by_length = (uint8_t)strlen("Backed by");
    uint8_t longest_size_of_t_length = (uint8_t)strlen("sizeof(T)");

    uint32_t total_backed_by  = 0;
    uint8_t  spacing          = 4;
    uint8_t  rows_length      = sizeof(rows) / sizeof(rows[0]);

    for (uint8_t r = 0; r < rows_length; r++)
    {
        uint8_t length = (uint8_t)(strlen(rows[r].name));
        if (longest_name_length < length)
            longest_name_length = length;
        
        length = digits_in_u32(rows[r].capacity);
        if (longest_capacity_length < length)
            longest_capacity_length = length;
        
        length = digits_in_u32(rows[r].used);
        if (longest_used_length < length)
            longest_used_length = length;

        length = (uint8_t)strlen(bytes_to_si(rows[r].backed_by));
        if (longest_backed_by_length < length)
            longest_backed_by_length = length;
        
        length = (uint8_t)strlen(bytes_to_si(rows[r].backed_by));
        if (longest_size_of_t_length < length)
            longest_size_of_t_length = length;
        
        total_backed_by += rows[r].backed_by;
    }
    
    for (uint8_t r = 0; r < rows_length; r++)
    {   
        if (!r || strcmp(rows[r - 1].group, rows[r].group))
        {
            if (r)
                putchar('\n');
            
            printf(rows[r].group);
            repeat_character(' ', longest_name_length - strlen(rows[r].group) + spacing);
 
            printf("Capacity");
            repeat_character(' ', longest_capacity_length - strlen("Capacity") + spacing);

            printf("Used");
            repeat_character(' ', longest_used_length - strlen("Used") + spacing);

            printf("Backed by");
            repeat_character(' ', longest_backed_by_length - strlen("Backed by") + spacing);

            printf("sizeof(T)\n");
        }
        
        printf(rows[r].name);
        repeat_character(' ', longest_name_length - strlen(rows[r].name) + spacing);

        printf("%u", rows[r].capacity);
        repeat_character(' ', longest_capacity_length - digits_in_u32(rows[r].capacity) + spacing);        

        printf("%u", rows[r].used);
        repeat_character(' ', longest_used_length - digits_in_u32(rows[r].used) + spacing);         

        const char* si = bytes_to_si(rows[r].backed_by);
        printf(si);
        repeat_character(' ', longest_backed_by_length - strlen(si) + spacing);
        
        printf("%s\n", bytes_to_si(rows[r].size_of_t));
    }
    
    printf("\nTotal");
    repeat_character(' ', longest_name_length + longest_capacity_length + spacing + longest_backed_by_length + spacing - strlen("Total") - 1);
    printf(bytes_to_si(total_backed_by));
    putchar('\n');
    
    printf("kB = 1000B\n"
           "MB = 1000kB\n"
           "GB = 1000MB\n");
}
