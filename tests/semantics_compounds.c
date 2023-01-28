#include "runner.h"

static void comparing_polymorphic_compound_with_non_matching_actual_type_parameters(void)
{
    format_test_name("semantics/compounds/comparing_polymorphic_compound_with_non_matching_actual_type_parameters");

    format_source_path(0, "semantics/compounds/comparing_polymorphic_compound_with_non_matching_actual_type_parameters.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T]\n"
                          "    T a\n"
                          "end\n"
                          "\n"
                          "function do_stuff(A[F64] a) : A[Bool]\n"
                          "    return a\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/comparing_polymorphic_compound_with_non_matching_actual_type_parameters.owen");
    format_expectation(&test.expected_error, "semantics/compounds/comparing_polymorphic_compound_with_non_matching_actual_type_parameters.owen:9:12: A[Bool] expected but found A[F64].\n");
}

static void directly_recursive_monomorphic_compound_through_fixed_sized_array(void)
{
    format_test_name("semantics/compounds/directly_recursive_monomorphic_compound_through_fixed_sized_array");

    format_source_path(0, "semantics/compounds/directly_recursive_monomorphic_compound_through_fixed_sized_array.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    [3]A a\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/directly_recursive_monomorphic_compound_through_fixed_sized_array.owen");
    format_expectation(&test.expected_error, "semantics/compounds/directly_recursive_monomorphic_compound_through_fixed_sized_array.owen:5:5: A is recursive.\n");
}

static void directly_recursive_monomorphic_compound_through_itself(void)
{
    format_test_name("semantics/compounds/directly_recursive_monomorphic_compound_through_itself");

    format_source_path(0, "semantics/compounds/directly_recursive_monomorphic_compound_through_itself.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    A a\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/directly_recursive_monomorphic_compound_through_itself.owen");
    format_expectation(&test.expected_error, "semantics/compounds/directly_recursive_monomorphic_compound_through_itself.owen:5:5: A is recursive.\n");
}

static void directly_recursive_polymorphic_compound_through_fixed_sized_array(void)
{
    format_test_name("semantics/compounds/directly_recursive_polymorphic_compound_through_fixed_sized_array");

    format_source_path(0, "semantics/compounds/directly_recursive_polymorphic_compound_through_fixed_sized_array.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T]\n"
                          "    [3]A[T] a\n"
                          "end\n"
                          "\n"
                          "structure B\n"
                          "    A[I32] a\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/directly_recursive_polymorphic_compound_through_fixed_sized_array.owen");
    format_expectation(&test.expected_error, 
                       "semantics/compounds/directly_recursive_polymorphic_compound_through_fixed_sized_array.owen:5:5: A[I32] is recursive.\n"
                       "Polymorphic stack:\n"
                       "    semantics/compounds/directly_recursive_polymorphic_compound_through_fixed_sized_array.owen:9:5: A[I32]\n");
}

static void directly_recursive_polymorphic_compound_through_itself(void)
{
    format_test_name("semantics/compounds/directly_recursive_polymorphic_compound_through_itself");

    format_source_path(0, "semantics/compounds/directly_recursive_polymorphic_compound_through_itself.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T]\n"
                          "    A[T] a\n"
                          "end\n"
                          "\n"
                          "structure B\n"
                          "    A[I32] a\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/directly_recursive_polymorphic_compound_through_itself.owen");
    format_expectation(&test.expected_error, "%s:5:5: A[I32] is recursive.\n"
                                             "Polymorphic stack:\n"
                                             "    semantics/compounds/directly_recursive_polymorphic_compound_through_itself.owen:9:5: A[I32]\n", test.source_paths[0]);
}

static void duplicate_formal_type_parameters(void)
{
    format_test_name("semantics/compounds/duplicate_formal_type_parameters");

    format_source_path(0, "semantics/compounds/duplicate_formal_type_parameters.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T, T]\n"
                          "    T x\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/duplicate_formal_type_parameters.owen");
    format_expectation(&test.expected_error, 
                       "semantics/compounds/duplicate_formal_type_parameters.owen:4:16: Duplicate formal type parameter T.\n");
}

static void formal_type_parameter_with_the_same_name_as_a_primitive(void)
{
    format_test_name("semantics/compounds/formal_type_parameter_with_the_same_name_as_a_primitive");

    format_source_path(0, "semantics/compounds/formal_type_parameter_with_the_same_name_as_a_primitive.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[I32]\n"
                          "    I32 x\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/formal_type_parameter_with_the_same_name_as_a_primitive.owen");
    format_expectation(&test.expected_error, 
                       "semantics/compounds/formal_type_parameter_with_the_same_name_as_a_primitive.owen:4:13: Formal type parameter matches I32.\n"
                       "Declarations:\n"
                       "    Primitive: I32\n"
                       "    semantics/compounds/formal_type_parameter_with_the_same_name_as_a_primitive.owen:4:11: A[I32]\n");
}

static void formal_type_parameter_with_the_same_name_as_an_enumeration(void)
{
    format_test_name("semantics/compounds/formal_type_parameter_with_the_same_name_as_an_enumeration");

    format_source_path(0, "semantics/compounds/formal_type_parameter_with_the_same_name_as_an_enumeration.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : I32\n"
                          "end\n"
                          "\n"
                          "structure B[A]\n"
                          "    A a\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/formal_type_parameter_with_the_same_name_as_an_enumeration.owen");
    format_expectation(&test.expected_error, 
                       "semantics/compounds/formal_type_parameter_with_the_same_name_as_an_enumeration.owen:7:13: Formal type parameter matches A.\n"
                       "Declarations:\n"
                       "    semantics/compounds/formal_type_parameter_with_the_same_name_as_an_enumeration.owen:4:13: A\n"
                       "    semantics/compounds/formal_type_parameter_with_the_same_name_as_an_enumeration.owen:7:11: B[A]\n");
}

static void formal_type_parameter_with_the_same_name_as_monomorphic_compound(void)
{
    format_test_name("semantics/compounds/formal_type_parameter_with_the_same_name_as_monomorphic_compound");

    format_source_path(0, "semantics/compounds/formal_type_parameter_with_the_same_name_as_monomorphic_compound.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    I32 x\n"
                          "end\n"
                          "\n"
                          "structure B[A]\n"
                          "    A a\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/formal_type_parameter_with_the_same_name_as_monomorphic_compound.owen");
    format_expectation(&test.expected_error, 
                       "semantics/compounds/formal_type_parameter_with_the_same_name_as_monomorphic_compound.owen:8:13: Formal type parameter matches A.\n"
                       "Declarations:\n"
                       "    semantics/compounds/formal_type_parameter_with_the_same_name_as_monomorphic_compound.owen:4:11: A\n"
                       "    semantics/compounds/formal_type_parameter_with_the_same_name_as_monomorphic_compound.owen:8:11: B[A]\n");
}

static void indirectly_recursive_monomorphic_compound_through_fixed_sized_array(void)
{
    format_test_name("semantics/compounds/indirectly_recursive_monomorphic_compound_through_fixed_sized_array");

    format_source_path(0, "semantics/compounds/indirectly_recursive_monomorphic_compound_through_fixed_sized_array.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    [3]B b\n"
                          "end\n"
                          "\n"
                          "structure B\n"
                          "    C c\n"
                          "end\n"
                          "\n"
                          "structure C\n"
                          "    A a\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/indirectly_recursive_monomorphic_compound_through_fixed_sized_array.owen");
    format_expectation(&test.expected_error, "%s:5:5: A is recursive.\n", test.source_paths[0]);
}

static void indirectly_recursive_monomorphic_compound_through_monomorphic_compound(void)
{
    format_test_name("semantics/compounds/indirectly_recursive_monomorphic_compound_through_monomorphic_compound");

    format_source_path(0, "semantics/compounds/indirectly_recursive_monomorphic_compound_through_monomorphic_compound.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    B b\n"
                          "end\n"
                          "\n"
                          "structure B\n"
                          "    C c\n"
                          "end\n"
                          "\n"
                          "structure C\n"
                          "    A a\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/indirectly_recursive_monomorphic_compound_through_monomorphic_compound.owen");
    format_expectation(&test.expected_error, "%s:5:5: A is recursive.\n", test.source_paths[0]);
}

static void indirectly_recursive_polymorphic_compound_through_fixed_sized_array(void)
{
    format_test_name("semantics/compounds/indirectly_recursive_polymorphic_compound_through_fixed_sized_array");

    format_source_path(0, "semantics/compounds/indirectly_recursive_polymorphic_compound_through_fixed_sized_array.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T]\n"
                          "    [3]B[T] b\n"
                          "end\n"
                          "\n"
                          "structure B[T]\n"
                          "    A[T] a\n"
                          "end\n"
                          "\n"
                          "structure C\n"
                          "    A[I32] a\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/indirectly_recursive_polymorphic_compound_through_fixed_sized_array.owen");
    format_expectation(&test.expected_error, 
                       "semantics/compounds/indirectly_recursive_polymorphic_compound_through_fixed_sized_array.owen:9:5: B[I32] is recursive.\n"
                       "Polymorphic stack:\n"
                       "    semantics/compounds/indirectly_recursive_polymorphic_compound_through_fixed_sized_array.owen:13:5: A[I32]\n"
                       "    semantics/compounds/indirectly_recursive_polymorphic_compound_through_fixed_sized_array.owen:5:8: B[I32]\n");
}

static void indirectly_recursive_monomorphic_compound_through_pointer(void)
{
    format_test_name("semantics/compounds/indirectly_recursive_monomorphic_compound_through_pointer");

    format_source_path(0, "semantics/compounds/indirectly_recursive_monomorphic_compound_through_pointer.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    #B b\n"
                          "end\n"
                          "\n"
                          "structure B\n"
                          "    C c\n"
                          "end\n"
                          "\n"
                          "structure C\n"
                          "    A a\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/indirectly_recursive_monomorphic_compound_through_pointer.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/compounds/indirectly_recursive_monomorphic_compound_through_pointer.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        field\n"
                       "            type: #B\n"
                       "            name: b\n"
                       "    structure\n"
                       "        name: B\n"
                       "        is_public: false\n"
                       "        field\n"
                       "            type: C\n"
                       "            name: c\n"
                       "    structure\n"
                       "        name: C\n"
                       "        is_public: false\n"
                       "        field\n"
                       "            type: A\n"
                       "            name: a\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void indirectly_recursive_polymorphic_compound_through_polymorphic_compound(void)
{
    format_test_name("semantics/compounds/indirectly_recursive_polymorphic_compound_through_polymorphic_compound");

    format_source_path(0, "semantics/compounds/indirectly_recursive_polymorphic_compound_through_polymorphic_compound.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T]\n"
                          "    B[T] b\n"
                          "end\n"
                          "\n"
                          "structure B[T]\n"
                          "    A[T] a\n"
                          "end\n"
                          "\n"
                          "structure C\n"
                          "    A[I32] a\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/indirectly_recursive_polymorphic_compound_through_polymorphic_compound.owen");
    format_expectation(&test.expected_error, 
                       "semantics/compounds/indirectly_recursive_polymorphic_compound_through_polymorphic_compound.owen:9:5: B[I32] is recursive.\n"
                       "Polymorphic stack:\n"
                       "    semantics/compounds/indirectly_recursive_polymorphic_compound_through_polymorphic_compound.owen:13:5: A[I32]\n"
                       "    semantics/compounds/indirectly_recursive_polymorphic_compound_through_polymorphic_compound.owen:5:5: B[I32]\n");
}

static void matching_polymorphic_compounds(void)
{
    format_test_name("semantics/compounds/matching_polymorphic_compounds");

    format_source_path(0, "semantics/compounds/matching_polymorphic_compounds.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T1]\n"
                          "    T1 x\n"
                          "end\n"
                          "\n"
                          "structure A[T2]\n"
                          "    T2 x\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/matching_polymorphic_compounds.owen");
    format_expectation(&test.expected_error, 
                       "semantics/compounds/matching_polymorphic_compounds.owen:4:11: Matching declarations for A in scope.\n"
                       "Declarations:\n"
                       "    semantics/compounds/matching_polymorphic_compounds.owen:4:11: A[T1]\n"
                       "    semantics/compounds/matching_polymorphic_compounds.owen:8:11: A[T2]\n");
}

static void monomorphic_compound_defines_multiple_fields_with_the_same_name(void)
{
    format_test_name("semantics/compounds/monomorphic_compound_defines_multiple_fields_with_the_same_name");

    format_source_path(0, "semantics/compounds/monomorphic_compound_defines_multiple_fields_with_the_same_name.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    I32 a\n"
                          "    I32 b\n"
                          "    I32 c\n"
                          "    I32 d\n"
                          "    I32 a\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/monomorphic_compound_defines_multiple_fields_with_the_same_name.owen");
    format_expectation(&test.expected_error, 
                       "semantics/compounds/monomorphic_compound_defines_multiple_fields_with_the_same_name.owen:9:9: Duplicate field name a.\n");
}

static void monomorphic_compounds_with_the_same_name_in_different_namespaces(void)
{
    format_test_name("semantics/compounds/monomorphic_compounds_with_the_same_name_in_different_namespaces");

    format_source_path(0, "semantics/compounds/monomorphic_compounds_with_the_same_name_in_different_namespaces-1.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    I32 a\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    format_source_path(1, "semantics/compounds/monomorphic_compounds_with_the_same_name_in_different_namespaces-2.owen");
    format_source_file(1, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Def\n"
                          "\n"
                          "structure A\n"
                          "    I32 a\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/monomorphic_compounds_with_the_same_name_in_different_namespaces-1.owen "
                     "semantics/compounds/monomorphic_compounds_with_the_same_name_in_different_namespaces-2.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/compounds/monomorphic_compounds_with_the_same_name_in_different_namespaces-1.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: a\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n"
                       "file\n"
                       "    path: semantics/compounds/monomorphic_compounds_with_the_same_name_in_different_namespaces-2.owen\n"
                       "    namespace: Def\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: a\n");
}

static void monomorphic_compounds_with_the_same_name_in_the_same_file(void)
{
    format_test_name("semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_file");

    format_source_path(0, "semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_file.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    I32 a\n"
                          "end\n"
                          "\n"
                          "structure A\n"
                          "    I32 a\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_file.owen");
    format_expectation(&test.expected_error,
                       "semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_file.owen:4:11: Matching declarations for A in scope.\n"
                       "Declarations:\n"
                       "    semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_file.owen:4:11: A\n"
                       "    semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_file.owen:8:11: A\n");
}

static void monomorphic_compounds_with_the_same_name_in_the_same_namespaces_but_both_are_non_public(void)
{
    format_test_name("semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_namespaces_but_both_are_non_public");

    format_source_path(0, "semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_namespaces_but_both_are_non_public-1.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    I32 a\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    format_source_path(1, "semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_namespaces_but_both_are_non_public-2.owen");
    format_source_file(1, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    I32 a\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_namespaces_but_both_are_non_public-1.owen "
                     "semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_namespaces_but_both_are_non_public-2.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_namespaces_but_both_are_non_public-1.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: a\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n"
                       "file\n"
                       "    path: semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_namespaces_but_both_are_non_public-2.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: a\n");
}

static void monomorphic_compounds_with_the_same_name_in_the_same_namespaces_where_one_is_public(void)
{
    format_test_name("semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_namespaces_where_one_is_public");

    format_source_path(0, "semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_namespaces_where_one_is_public-1.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "public structure A\n"
                          "    I32 a\n"
                          "end");

    format_source_path(1, "semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_namespaces_where_one_is_public-2.owen");
    format_source_file(1, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    I32 a\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_namespaces_where_one_is_public-1.owen "
                     "semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_namespaces_where_one_is_public-2.owen "
                     "-print-semantics");

    format_expectation(&test.expected_error, 
                       "semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_namespaces_where_one_is_public-2.owen:4:11: Matching declarations for A in scope.\n"
                       "Declarations:\n"
                       "    semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_namespaces_where_one_is_public-2.owen:4:11: A\n"
                       "    semantics/compounds/monomorphic_compounds_with_the_same_name_in_the_same_namespaces_where_one_is_public-1.owen:4:18: A\n");
}

static void nested_non_recursive_polymorphic_compounds(void)
{
    format_test_name("semantics/compounds/nested_non_recursive_polymorphic_compounds");

    format_source_path(0, "semantics/compounds/nested_non_recursive_polymorphic_compounds.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T]\n"
                          "    B[T] b\n"
                          "end\n"
                          "\n"
                          "structure B[T]\n"
                          "    T x\n"
                          "end\n"
                          "\n"
                          "function do_stuff(A[I32] a) : A[I32]\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/nested_non_recursive_polymorphic_compounds.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/compounds/nested_non_recursive_polymorphic_compounds.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        formal_type_parameter: T\n"
                       "        field\n"
                       "            type_reference: B[T]\n"
                       "            name: b\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        actual_type_parameter: I32\n"
                       "        field\n"
                       "            type: B[I32]\n"
                       "            name: b\n"
                       "    structure\n"
                       "        name: B\n"
                       "        is_public: false\n"
                       "        formal_type_parameter: T\n"
                       "        field\n"
                       "            type_reference: T\n"
                       "            name: x\n"
                       "    structure\n"
                       "        name: B\n"
                       "        is_public: false\n"
                       "        actual_type_parameter: I32\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: x\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_parameter\n"
                       "            type: A[I32]\n"
                       "            name: a\n"
                       "        return_type: A[I32]\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: A[I32]\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void non_recursive_monomorphic_compound_field(void)
{
    format_test_name("semantics/compounds/non_recursive_monomorphic_compound_field");

    format_source_path(0, "semantics/compounds/non_recursive_monomorphic_compound_field.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    B b\n"
                          "end\n"
                          "\n"
                          "structure B\n"
                          "    I32 x\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/non_recursive_monomorphic_compound_field.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/compounds/non_recursive_monomorphic_compound_field.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        field\n"
                       "            type: B\n"
                       "            name: b\n"
                       "    structure\n"
                       "        name: B\n"
                       "        is_public: false\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: x\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void polymorphic_compound_defines_multiple_fields_with_the_same_name(void)
{
    format_test_name("semantics/compounds/polymorphic_compound_defines_multiple_fields_with_the_same_name");

    format_source_path(0, "semantics/compounds/polymorphic_compound_defines_multiple_fields_with_the_same_name.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T]\n"
                          "    T a\n"
                          "    T b\n"
                          "    T c\n"
                          "    T d\n"
                          "    T a\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/polymorphic_compound_defines_multiple_fields_with_the_same_name.owen");
    format_expectation(&test.expected_error, 
                       "semantics/compounds/polymorphic_compound_defines_multiple_fields_with_the_same_name.owen:9:7: Duplicate field name a.\n");
}

static void polymorphic_compound_with_formal_type_parameter_with_the_same_name(void)
{
    format_test_name("semantics/compounds/polymorphic_compound_with_formal_type_parameter_with_the_same_name");

    format_source_path(0, "semantics/compounds/polymorphic_compound_with_formal_type_parameter_with_the_same_name.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "// This is definitely one of those weird fringe cases\n"
                          "// but it is allowed since there is no primitive,\n"
                          "// enumeration or monomorphic compound named A\n"
                          "// in scope.\n"
                          "structure A[A]\n"
                          "    A x\n"
                          "end\n"
                          "\n"
                          "function do_stuff(A[I32] a) : A[I32]\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/polymorphic_compound_with_formal_type_parameter_with_the_same_name.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/compounds/polymorphic_compound_with_formal_type_parameter_with_the_same_name.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        formal_type_parameter: A\n"
                       "        field\n"
                       "            type_reference: A\n"
                       "            name: x\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        actual_type_parameter: I32\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: x\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_parameter\n"
                       "            type: A[I32]\n"
                       "            name: a\n"
                       "        return_type: A[I32]\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: A[I32]\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void polymorphic_compounds_with_the_same_name_but_different_amount_of_formal_type_parameters(void)
{
    format_test_name("semantics/compounds/polymorphic_compounds_with_the_same_name_but_different_amount_of_formal_type_parameters");

    format_source_path(0, "semantics/compounds/polymorphic_compounds_with_the_same_name_but_different_amount_of_formal_type_parameters.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    I32 x\n"
                          "    I32 y\n"
                          "end\n"
                          "\n"
                          "structure A[T1]\n"
                          "    T1 x\n"
                          "    T1 y\n"
                          "end\n"
                          "\n"
                          "structure A[T1, T2]\n"
                          "    T1 x\n"
                          "    T2 y\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/polymorphic_compounds_with_the_same_name_but_different_amount_of_formal_type_parameters.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/compounds/polymorphic_compounds_with_the_same_name_but_different_amount_of_formal_type_parameters.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: x\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: y\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        formal_type_parameter: T1\n"
                       "        field\n"
                       "            type_reference: T1\n"
                       "            name: x\n"
                       "        field\n"
                       "            type_reference: T1\n"
                       "            name: y\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        formal_type_parameter: T1\n"
                       "        formal_type_parameter: T2\n"
                       "        field\n"
                       "            type_reference: T1\n"
                       "            name: x\n"
                       "        field\n"
                       "            type_reference: T2\n"
                       "            name: y\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void public_compound_with_field_of_non_public_compound(void)
{
    format_test_name("semantics/compounds/public_compound_with_field_of_non_public_compound");

    format_source_path(0, "semantics/compounds/public_compound_with_field_of_non_public_compound.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "public structure A\n"
                          "    B b\n"
                          "end\n"
                          "\n"
                          "structure B\n"
                          "    I32 x\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/public_compound_with_field_of_non_public_compound.owen");
    format_expectation(&test.expected_error, 
                       "semantics/compounds/public_compound_with_field_of_non_public_compound.owen:5:5: A exposes B which is not public.\n");
}

static void public_compound_with_field_of_non_public_enumeration(void)
{
    format_test_name("semantics/compounds/public_compound_with_field_of_non_public_enumeration");

    format_source_path(0, "semantics/compounds/public_compound_with_field_of_non_public_enumeration.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "public structure A\n"
                          "    B b\n"
                          "end\n"
                          "\n"
                          "enumeration B : I32\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/public_compound_with_field_of_non_public_enumeration.owen");
    format_expectation(&test.expected_error, 
                       "semantics/compounds/public_compound_with_field_of_non_public_enumeration.owen:5:5: A exposes B which is not public.\n");
}

static void public_compound_with_field_of_public_compound(void)
{
    format_test_name("semantics/compounds/public_compound_with_field_of_public_compound");

    format_source_path(0, "semantics/compounds/public_compound_with_field_of_public_compound.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "public structure A\n"
                          "    B b\n"
                          "end\n"
                          "\n"
                          "public structure B\n"
                          "    I32 x\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/public_compound_with_field_of_public_compound.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/compounds/public_compound_with_field_of_public_compound.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: true\n"
                       "        field\n"
                       "            type: B\n"
                       "            name: b\n"
                       "    structure\n"
                       "        name: B\n"
                       "        is_public: true\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: x\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void public_compound_with_field_of_public_enumeration(void)
{
    format_test_name("semantics/compounds/public_compound_with_field_of_public_enumeration");

    format_source_path(0, "semantics/compounds/public_compound_with_field_of_public_enumeration.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "public structure A\n"
                          "    B b\n"
                          "end\n"
                          "\n"
                          "public enumeration B : I32\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/public_compound_with_field_of_public_enumeration.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/compounds/public_compound_with_field_of_public_enumeration.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: true\n"
                       "        field\n"
                       "            type: B\n"
                       "            name: b\n"
                       "    enumeration\n"
                       "        name: B\n"
                       "        underlying_type: I32\n"
                       "        is_public: true\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void missing_actual_type_parameter_in_polymorphic_field_type(void)
{
    format_test_name("semantics/compounds/missing_actual_type_parameter_in_polymorphic_field_type");

    format_source_path(0, "semantics/compounds/missing_actual_type_parameter_in_polymorphic_field_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T]\n"
                          "    A a\n"
                          "end\n"
                          "\n"
                          "structure B\n"
                          "    A[I32] a\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/missing_actual_type_parameter_in_polymorphic_field_type.owen");
    format_expectation(&test.expected_error,
                       "semantics/compounds/missing_actual_type_parameter_in_polymorphic_field_type.owen:5:5: Undefined type referenced.\n"
                       "Polymorphic stack:\n"
                       "    semantics/compounds/missing_actual_type_parameter_in_polymorphic_field_type.owen:9:5: A[I32]\n");
}

static void monomorphic_field_type_is_undefined_in_monomorphic_compound(void)
{
    format_test_name("semantics/compounds/monomorphic_field_type_is_undefined_in_monomorphic_compound");

    format_source_path(0, "semantics/compounds/monomorphic_field_type_is_undefined_in_monomorphic_compound.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    B b\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/monomorphic_field_type_is_undefined_in_monomorphic_compound.owen");
    format_expectation(&test.expected_error, 
                       "semantics/compounds/monomorphic_field_type_is_undefined_in_monomorphic_compound.owen:5:5: Undefined type referenced.\n");
}

static void polymorphic_type_using_formal_type_parameter(void)
{
    format_test_name("semantics/compounds/polymorphic_type_using_formal_type_parameter");

    format_source_path(0, "semantics/compounds/polymorphic_type_using_formal_type_parameter.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T]\n"
                          "    B[T] a\n"
                          "end\n"
                          "\n"
                          "structure B[T]\n"
                          "    T a\n"
                          "end\n"
                          "\n"
                          "structure C\n"
                          "    A[I32] x\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/polymorphic_type_using_formal_type_parameter.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/compounds/polymorphic_type_using_formal_type_parameter.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        formal_type_parameter: T\n"
                       "        field\n"
                       "            type_reference: B[T]\n"
                       "            name: a\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        actual_type_parameter: I32\n"
                       "        field\n"
                       "            type: B[I32]\n"
                       "            name: a\n"
                       "    structure\n"
                       "        name: B\n"
                       "        is_public: false\n"
                       "        formal_type_parameter: T\n"
                       "        field\n"
                       "            type_reference: T\n"
                       "            name: a\n"
                       "    structure\n"
                       "        name: B\n"
                       "        is_public: false\n"
                       "        actual_type_parameter: I32\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: a\n"
                       "    structure\n"
                       "        name: C\n"
                       "        is_public: false\n"
                       "        field\n"
                       "            type: A[I32]\n"
                       "            name: x\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void pointer_to_formal_type_parameter(void)
{
    format_test_name("semantics/compounds/pointer_to_formal_type_parameter");

    format_source_path(0, "semantics/compounds/pointer_to_formal_type_parameter.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T]\n"
                          "    #T a\n"
                          "end\n"
                          "\n"
                          "structure B\n"
                          "    A[I32] x\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/pointer_to_formal_type_parameter.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/compounds/pointer_to_formal_type_parameter.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        formal_type_parameter: T\n"
                       "        field\n"
                       "            type_reference: #T\n"
                       "            name: a\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        actual_type_parameter: I32\n"
                       "        field\n"
                       "            type: #I32\n"
                       "            name: a\n"
                       "    structure\n"
                       "        name: B\n"
                       "        is_public: false\n"
                       "        field\n"
                       "            type: A[I32]\n"
                       "            name: x\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void dynamic_array_of_formal_type_parameter(void)
{
    format_test_name("semantics/compounds/dynamic_array_of_formal_type_parameter");

    format_source_path(0, "semantics/compounds/dynamic_array_of_formal_type_parameter.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T]\n"
                          "    []T a\n"
                          "end\n"
                          "\n"
                          "structure B\n"
                          "    A[I32] x\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/dynamic_array_of_formal_type_parameter.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/compounds/dynamic_array_of_formal_type_parameter.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        formal_type_parameter: T\n"
                       "        field\n"
                       "            type_reference: []T\n"
                       "            name: a\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        actual_type_parameter: I32\n"
                       "        field\n"
                       "            type: []I32\n"
                       "            name: a\n"
                       "    structure\n"
                       "        name: B\n"
                       "        is_public: false\n"
                       "        field\n"
                       "            type: A[I32]\n"
                       "            name: x\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void fixed_array_of_formal_type_parameter(void)
{
    format_test_name("semantics/compounds/fixed_array_of_formal_type_parameter");

    format_source_path(0, "semantics/compounds/fixed_array_of_formal_type_parameter.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T]\n"
                          "    [2]T a\n"
                          "end\n"
                          "\n"
                          "structure B\n"
                          "    A[I32] x\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end\n");

    SET_COMMAND_LINE("semantics/compounds/fixed_array_of_formal_type_parameter.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/compounds/fixed_array_of_formal_type_parameter.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        formal_type_parameter: T\n"
                       "        field\n"
                       "            type_reference: [2]T\n"
                       "            name: a\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        actual_type_parameter: I32\n"
                       "        field\n"
                       "            type: [2]I32\n"
                       "            name: a\n"
                       "    structure\n"
                       "        name: B\n"
                       "        is_public: false\n"
                       "        field\n"
                       "            type: A[I32]\n"
                       "            name: x\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void nested_formal_type_parameter(void)
{
    format_test_name("semantics/compounds/nested_formal_type_parameter");

    format_source_path(0, "semantics/compounds/nested_formal_type_parameter.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T]\n"
                          "    T[I32] b\n"
                          "end");

    SET_COMMAND_LINE("semantics/compounds/nested_formal_type_parameter.owen");
    format_expectation(&test.expected_error, 
                       "semantics/compounds/nested_formal_type_parameter.owen:5:5: Type parameters can't be nested.\n");
}

static bool qualified_field(void)
{    
    static uint8_t state = 0;
    switch (state++)
    {
        case 0:
            format_test_name("semantics/compounds/monomorphic_readonly_qualified_field");

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "structure S\n"
                                  "    readonly #I32 a\n"
                                  "end");

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:5:5: Field types cannot be qualified.\n",
                               test.source_paths[0]);

            break;

        case 1:
            format_test_name("semantics/compounds/monomorphic_noalias_qualified_field");

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "structure S\n"
                                  "    noalias #I32 a\n"
                                  "end");

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:5:5: Only Pointerless formal parameter types can be noalias qualified.\n",
                               test.source_paths[0]);

            break;
    
        case 2:
            format_test_name("semantics/compounds/monomorphic_readonly_noalias_qualified_field");

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "structure S\n"
                                  "    readonly noalias #I32 a\n"
                                  "end");

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:5:5: Only Pointerless formal parameter types can be noalias qualified.\n",
                               test.source_paths[0]);

            break;

        case 3:
            format_test_name("semantics/compounds/polymorphic_readonly_qualified_field");

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "structure S[T]\n"
                                  "    readonly #I32 a\n"
                                  "end\n"
                                  "\n"
                                  "structure S\n"
                                  "    S[#I32] s\n"
                                  "end\n");

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:5:5: Field types cannot be qualified.\n"
                               "Polymorphic stack:\n"
                               "    %s:9:5: S[#I32]\n",
                               test.source_paths[0],
                               test.source_paths[0]);

            break;

        case 4:
            format_test_name("semantics/compounds/polymorphic_noalias_qualified_field");

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "structure S[T]\n"
                                  "    noalias #I32 a\n"
                                  "end\n"
                                  "\n"
                                  "structure S\n"
                                  "    S[#I32] s\n"
                                  "end\n");

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:5:5: Only Pointerless formal parameter types can be noalias qualified.\n"
                               "Polymorphic stack:\n"
                               "    %s:9:5: S[#I32]\n",
                               test.source_paths[0],
                               test.source_paths[0]);

            break;
    
        case 5:
            format_test_name("semantics/compounds/polymorphic_readonly_noalias_qualified_field");

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "structure S[T]\n"
                                  "    readonly noalias #I32 a\n"
                                  "end\n"
                                  "\n"
                                  "structure S\n"
                                  "    S[#I32] s\n"
                                  "end\n");

            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:5:5: Only Pointerless formal parameter types can be noalias qualified.\n"
                               "Polymorphic stack:\n"
                               "    %s:9:5: S[#I32]\n",
                               test.source_paths[0],
                               test.source_paths[0]);

            break;

        case 6:
            format_test_name("semantics/compounds/readonly_qualified_actual_type_parameter_as_field_type");

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "structure S[T]\n"
                                  "    T a\n"
                                  "end\n"
                                  "\n"
                                  "structure S\n"
                                  "    S[readonly #I32] s\n"
                                  "end\n");
            
            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:5:5: Field types cannot be qualified.\n"
                               "Polymorphic stack:\n"
                               "    %s:9:5: S[readonly #I32]\n",
                               test.source_paths[0],
                               test.source_paths[0]);

            break;

        case 7:
            format_test_name("semantics/compounds/noalias_qualified_actual_type_parameter_as_field_type");

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "structure S[T]\n"
                                  "    T a\n"
                                  "end\n"
                                  "\n"
                                  "structure S\n"
                                  "    S[noalias #I32] s\n"
                                  "end\n");
            
            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:5:5: Only Pointerless formal parameter types can be noalias qualified.\n"
                               "Polymorphic stack:\n"
                               "    %s:9:5: S[noalias #I32]\n",
                               test.source_paths[0],
                               test.source_paths[0]);

            break;
    
        case 8:
            format_test_name("semantics/compounds/readonly_noalias_qualified_actual_type_parameter_as_field_type");

            format_source_path(0, "%s.owen", test.name);
            format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                                  "namespace Abc\n"
                                  "\n"
                                  "structure S[T]\n"
                                  "    T a\n"
                                  "end\n"
                                  "\n"
                                  "structure S\n"
                                  "    S[readonly noalias #I32] s\n"
                                  "end\n");
            
            format_command_line_options(test.source_paths[0]);
            format_expectation(&test.expected_error, 
                               "%s:5:5: Only Pointerless formal parameter types can be noalias qualified.\n"
                               "Polymorphic stack:\n"
                               "    %s:9:5: S[readonly noalias #I32]\n",
                               test.source_paths[0],
                               test.source_paths[0]);

            break;
        
        default:
            return false;
    }
    
    return true;
}

bool semantics_compounds(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            comparing_polymorphic_compound_with_non_matching_actual_type_parameters();
            state++;
            break;

        case 1:
            directly_recursive_monomorphic_compound_through_fixed_sized_array();
            state++;
            break;

        case 2:
            directly_recursive_monomorphic_compound_through_itself();
            state++;
            break;

        case 3:
            directly_recursive_polymorphic_compound_through_fixed_sized_array();
            state++;
            break;

        case 4:
            directly_recursive_polymorphic_compound_through_itself();
            state++;
            break;

        case 5:
            duplicate_formal_type_parameters();
            state++;
            break;

        case 6:
            formal_type_parameter_with_the_same_name_as_a_primitive();
            state++;
            break;

        case 7:
            formal_type_parameter_with_the_same_name_as_an_enumeration();
            state++;
            break;

        case 8:
            formal_type_parameter_with_the_same_name_as_monomorphic_compound();
            state++;
            break;

        case 9:
            indirectly_recursive_monomorphic_compound_through_fixed_sized_array();
            state++;
            break;

        case 10:
            indirectly_recursive_monomorphic_compound_through_monomorphic_compound();
            state++;
            break;

        case 11:
            indirectly_recursive_polymorphic_compound_through_fixed_sized_array();
            state++;
            break;

        case 12:
            indirectly_recursive_monomorphic_compound_through_pointer();
            state++;
            break;

        case 13:
            indirectly_recursive_polymorphic_compound_through_polymorphic_compound();
            state++;
            break;

        case 14:
            matching_polymorphic_compounds();
            state++;
            break;

        case 15:
            monomorphic_compound_defines_multiple_fields_with_the_same_name();
            state++;
            break;

        case 16:
            monomorphic_compounds_with_the_same_name_in_different_namespaces();
            state++;
            break;

        case 17:
            monomorphic_compounds_with_the_same_name_in_the_same_file();
            state++;
            break;

        case 18:
            monomorphic_compounds_with_the_same_name_in_the_same_namespaces_but_both_are_non_public();
            state++;
            break;

        case 19:
            monomorphic_compounds_with_the_same_name_in_the_same_namespaces_where_one_is_public();
            state++;
            break;

        case 20:
            nested_non_recursive_polymorphic_compounds();
            state++;
            break;

        case 21:
            non_recursive_monomorphic_compound_field();
            state++;
            break;

        case 22:
            polymorphic_compound_defines_multiple_fields_with_the_same_name();
            state++;
            break;

        case 23:
            polymorphic_compound_with_formal_type_parameter_with_the_same_name();
            state++;
            break;

        case 24:
            polymorphic_compounds_with_the_same_name_but_different_amount_of_formal_type_parameters();
            state++;
            break;

        case 25:
            public_compound_with_field_of_non_public_compound();
            state++;
            break;

        case 26:
            public_compound_with_field_of_non_public_enumeration();
            state++;
            break;

        case 27:
            public_compound_with_field_of_public_compound();
            state++;
            break;

        case 28:
            public_compound_with_field_of_public_enumeration();
            state++;
            break;

        case 29:
            missing_actual_type_parameter_in_polymorphic_field_type();
            state++;
            break;

        case 30:
            monomorphic_field_type_is_undefined_in_monomorphic_compound();
            state++;
            break;

        case 31:
            polymorphic_type_using_formal_type_parameter();
            state++;
            break;

        case 32:
            pointer_to_formal_type_parameter();
            state++;
            break;

        case 33:
            dynamic_array_of_formal_type_parameter();
            state++;
            break;

        case 34:
            fixed_array_of_formal_type_parameter();
            state++;
            break;

        case 35:
            nested_formal_type_parameter();
            state++;
            break;

        case 36:
            if (qualified_field())
                break;
            
            state++;
        
        default:
            return false;
    }

    return true;
}
