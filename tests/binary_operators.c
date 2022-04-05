#include "runner.h"
#include <assert.h>

Binary_Operator binary_operators[BINARY_OPERATORS_LENGTH] =
{
    [0]  = { Binary_Operator_Tag_logical_or,            "||", false, "logical_or"            },
    [1]  = { Binary_Operator_Tag_logical_and,           "&&", false, "logical_and"           },
    [2]  = { Binary_Operator_Tag_equal_equal,           "==", true,  "equal_equal"           },
    [3]  = { Binary_Operator_Tag_not_equal,             "!=", true,  "not_equal"             },
    [4]  = { Binary_Operator_Tag_less_than_or_equal,    "<=", true,  "less_than_or_equal"    },
    [5]  = { Binary_Operator_Tag_greater_than_or_equal, ">=", true,  "greater_than_or_equal" },
    [6]  = { Binary_Operator_Tag_less_than,             "<",  true,  "less_than"             },
    [7]  = { Binary_Operator_Tag_greater_than,          ">",  true,  "greater_than"          },
    [8]  = { Binary_Operator_Tag_plus,                  "+",  false, "plus"                  },
    [9]  = { Binary_Operator_Tag_minus,                 "-",  false, "minus"                 },
    [10] = { Binary_Operator_Tag_bitwise_or,            "|",  false, "bitwise_or"            },
    [11] = { Binary_Operator_Tag_bitwise_xor,           "^",  false, "bitwise_xor"           },
    [12] = { Binary_Operator_Tag_multiply,              "*",  false, "multiply"              },
    [13] = { Binary_Operator_Tag_divide,                "/",  false, "divide"                },
    [14] = { Binary_Operator_Tag_modulo,                "%",  false, "modulo"                },
    [15] = { Binary_Operator_Tag_bitwise_and,           "&",  false, "bitwise_and"           },
    [16] = { Binary_Operator_Tag_left_shift,            "<<", false, "left_shift"            },
    [17] = { Binary_Operator_Tag_right_shift,           ">>", false, "right_shift"           }
};

uint8_t index_of_binary_operator(Binary_Operator_Tag tag)
{
    for (uint8_t i = 0; i < sizeof(binary_operators) / sizeof(binary_operators[0]); i++)
    {
        if (binary_operators[i].tag == tag)
            return i;
    }

    assert(false);
    return 0;
}

Binary_Operator_Tag next_binary_operator(Binary_Operator_Tag tag)
{
    if (tag == Binary_Operator_Tag_right_shift)
        return tag;
    else if (tag == Binary_Operator_Tag_logical_or)
        return Binary_Operator_Tag_logical_and;
    else
        return tag * 2;
}

bool is_binary_operator_defined_for_type(uint8_t type_index, Binary_Operator_Tag tag)
{
    return (tag & types[type_index].operators_defined_for_type) == tag;
}
