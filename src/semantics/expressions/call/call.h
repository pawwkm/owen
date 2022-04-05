#include "../expression.h"
void resolve_call_to_function_overload_by_actual_type_parameters(const File* file, Interned_String_Handle function_name, Span function_name_span, Call* call);
void resolve_call_to_function_overload_by_actual_parameters(const File* file, Interned_String_Handle function_name, Span function_name_span, Call* call);
void call_to_function_typed_expression(const File* file, Call* call, Expression_Check_Flags flags);

void resolve_type_of_call(Call* call, Type_Handle signature_handle);

bool signature_matches_actual_parameter_types(const Function* function, const Expression_Handle_Array* actual_parameters);
