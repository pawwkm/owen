#include "runner.h"

Assignment_Operator assignment_operators[] =
{
    [Assignment_Operator_Tag_equal]             = { "=",   "equal"             },
    [Assignment_Operator_Tag_plus_equal]        = { "+=",  "plus_equal"        },
    [Assignment_Operator_Tag_minus_equal]       = { "-=",  "minus_equal"       },
    [Assignment_Operator_Tag_bitwise_or_equal]  = { "|=",  "bitwise_or_equal"  },
    [Assignment_Operator_Tag_bitwise_xor_equal] = { "^=",  "bitwise_xor_equal" },
    [Assignment_Operator_Tag_multiply_equal]    = { "*=",  "multiply_equal"    },
    [Assignment_Operator_Tag_divide_equal]      = { "/=",  "divide_equal"      },
    [Assignment_Operator_Tag_modulo_equal]      = { "%=",  "modulo_equal"      },
    [Assignment_Operator_Tag_bitwise_and_equal] = { "&=",  "bitwise_and_equal" },
    [Assignment_Operator_Tag_left_shift_equal]  = { "<<=", "left_shift_equal"  },
    [Assignment_Operator_Tag_right_shift_equal] = { ">>=", "right_shift_equal" }
};

bool is_assignment_operator_defined_for_type(uint8_t type_index, Assignment_Operator_Tag tag)
{
    if (tag == Assignment_Operator_Tag_equal)
        return true;
    
    Binary_Operator_Tag binary_tag = Binary_Operator_Tag_logical_or;
    switch (tag)
    {
        case Assignment_Operator_Tag_plus_equal:
            binary_tag = Binary_Operator_Tag_plus;
            break;
            
        case Assignment_Operator_Tag_minus_equal:
            binary_tag = Binary_Operator_Tag_minus;
            break;
            
        case Assignment_Operator_Tag_bitwise_or_equal:
            binary_tag = Binary_Operator_Tag_bitwise_or;
            break;
            
        case Assignment_Operator_Tag_bitwise_xor_equal:
            binary_tag = Binary_Operator_Tag_bitwise_xor;
            break;
            
        case Assignment_Operator_Tag_multiply_equal:
            binary_tag = Binary_Operator_Tag_multiply;
            break;
            
        case Assignment_Operator_Tag_divide_equal:
            binary_tag = Binary_Operator_Tag_divide;
            break;
            
        case Assignment_Operator_Tag_modulo_equal:
            binary_tag = Binary_Operator_Tag_modulo;
            break;
            
        case Assignment_Operator_Tag_bitwise_and_equal:
            binary_tag = Binary_Operator_Tag_bitwise_and;
            break;
            
        case Assignment_Operator_Tag_left_shift_equal:
            binary_tag = Binary_Operator_Tag_left_shift;
            break;
            
        case Assignment_Operator_Tag_right_shift_equal:
            binary_tag = Binary_Operator_Tag_right_shift;
            break;
    }
    
    return is_binary_operator_defined_for_type(type_index, binary_tag);
}
