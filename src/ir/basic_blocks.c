#include "ir.h"

static int32_t index_of_definition(const Ir_Basic_Block* block, Interned_String_Handle name)
{
    for (int32_t i = 0; i < block->definitions_length; i++)
    {
        if (compare_interned_strings(name, block->definitions[i].name))
            return i;
    }

    return -1;
}

void write_definition(Ir_Basic_Block* block, Ir_Definition definition)
{
    int32_t i = index_of_definition(block, definition.name);
    if (i == -1)
        add_definition(block, definition);
    else
        block->definitions[i] = definition;
}

static Ir_Operand_Handle read_definition_recursively(const Ir_Basic_Block* block, Interned_String_Handle name)
{
    (void)block;
    (void)name;

    not_implemented(__FILE__, __LINE__, "Global value numbering");

    return invalid_ir_operand_handle;
}

Ir_Operand_Handle read_definition(const Ir_Basic_Block* block, Interned_String_Handle name)
{
    int32_t i = index_of_definition(block, name);
    if (i == -1)
        return read_definition_recursively(block, name);
    else
        // Local value numbering.
        return block->definitions[i].value;
}

