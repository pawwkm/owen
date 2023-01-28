#include "expression.h"

void type_check_enumeration_constant_access(const File* file, Enumeration_Constant_Access* access)
{
    Type_Handle handle = lookup_type_by_reference(file, access->enumeration, true, true);
    Enumeration_Type* enumeration = &lookup(handle)->enumeration;
    
    if (enumeration->tag != Type_Tag_enumeration)
        print_span_error(file, lookup(access->enumeration)->span, "%t is not an enumeration.", handle);

    lookup_constant_by_name(enumeration, access->constant, access->constant_span);
    access->type = handle;
}

void deep_copy_enumeration_constant_access(Enumeration_Constant_Access* restrict destination, const Enumeration_Constant_Access* restrict source)
{
    destination->enumeration = add_type_reference();
    deep_copy_type_reference(destination->enumeration, source->enumeration);

    destination->constant = source->constant;
}
