#include "ast_lowering_pass.h"

void lower_ast(void)
{
    lower_function(lookup_function(main_function));
}
