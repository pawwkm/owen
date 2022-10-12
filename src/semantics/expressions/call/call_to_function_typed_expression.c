#include "call.h"

void call_to_function_typed_expression(const File* file, Call* call, Expression_Check_Flags flags)
{
    Expression* callee = lookup_expression(call->callee);
    Function_Type* signature = &lookup_type(type_check_expression(file, callee, invalid_type_handle, flags))->function;
    if (signature->tag != Type_Tag_function)
        print_span_error(file, callee->span, "Function type expected but found %t.", callee->type);

    if (signature->formal_parameters.handles_length != call->actual_parameters.handles_length)
        print_span_error(file, callee->span, "Expected %u %s but found %u.",
                                             signature->formal_parameters.handles_length,
                                             signature->formal_parameters.handles_length == 1 ? "expression" : "expressions",
                                             call->actual_parameters.handles_length);

    for (Array_Size i = 0; i < signature->formal_parameters.handles_length; i++)
    {
        Type_Handle expected_type = type_at(&signature->formal_parameters, i);
        Expression* actual_parameter = lookup_expression(expression_at(&call->actual_parameters, i));
        if (!expression_types_match(expected_type, actual_parameter->type))
            print_span_error(file, actual_parameter->span, "%t expected but found %t.", expected_type, actual_parameter->type);
    }

    resolve_type_of_call(call, callee->type);
}
