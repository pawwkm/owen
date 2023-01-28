#include "runner.h"

static void duplicate_formal_type_parameters(void)
{
    format_test_name("semantics/functions/duplicate_formal_type_parameters");

    format_source_path(0, "semantics/functions/duplicate_formal_type_parameters.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function do_stuff[T, T, T](T a) : T\n"
                          "    return a\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/duplicate_formal_type_parameters.owen");
    format_expectation(&test.expected_error, 
                       "%s:4:22: Duplicate formal type parameter T.\n",
                       test.source_paths[0]);
}

static void formal_parameter_with_the_same_name_as_a_function(void)
{
    format_test_name("semantics/functions/formal_parameter_with_the_same_name_as_a_function");

    format_source_path(0, "semantics/functions/formal_parameter_with_the_same_name_as_a_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "end\n"
                          "\n"
                          "function b(I32 a) : I32\n"
                          "    return a\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/formal_parameter_with_the_same_name_as_a_function.owen");
    format_expectation(&test.expected_error, 
                       "%s:7:16: a is already defined.\n",
                       test.source_paths[0]);
}

static void formal_type_parameter_with_the_same_name_as_a_primitive(void)
{
    format_test_name("semantics/functions/formal_type_parameter_with_the_same_name_as_a_primitive");

    format_source_path(0, "semantics/functions/formal_type_parameter_with_the_same_name_as_a_primitive.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function do_stuff[I32](I32 a) : I32\n"
                          "    return a\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/formal_type_parameter_with_the_same_name_as_a_primitive.owen");
    format_expectation(&test.expected_error, 
                       "semantics/functions/formal_type_parameter_with_the_same_name_as_a_primitive.owen:4:19: Formal type parameter matches declaration for I32 in scope.\n"
                       "Declarations:\n"
                       "    semantics/functions/formal_type_parameter_with_the_same_name_as_a_primitive.owen:4:10: do_stuff[I32](I32 a) : I32\n"
                       "    Primitive: I32\n");
}

static void formal_type_parameter_with_the_same_name_as_an_enumeration(void)
{
    format_test_name("semantics/functions/formal_type_parameter_with_the_same_name_as_an_enumeration");

    format_source_path(0, "semantics/functions/formal_type_parameter_with_the_same_name_as_an_enumeration.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : U8\n"
                          "end\n"
                          "\n"
                          "function do_stuff[A](A a) : A\n"
                          "    return a\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/formal_type_parameter_with_the_same_name_as_an_enumeration.owen");
    format_expectation(&test.expected_error, 
                       "semantics/functions/formal_type_parameter_with_the_same_name_as_an_enumeration.owen:7:19: Formal type parameter matches declaration for A in scope.\n"
                       "Declarations:\n"
                       "    semantics/functions/formal_type_parameter_with_the_same_name_as_an_enumeration.owen:7:10: do_stuff[A](A a) : A\n"
                       "    semantics/functions/formal_type_parameter_with_the_same_name_as_an_enumeration.owen:4:13: A\n");
}

static void formal_type_parameter_with_the_same_name_as_polymorphic_compound(void)
{
    format_test_name("semantics/functions/formal_type_parameter_with_the_same_name_as_polymorphic_compound");

    format_source_path(0, "semantics/functions/formal_type_parameter_with_the_same_name_as_polymorphic_compound.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A[T]\n"
                          "    T x\n"
                          "end\n"
                          "\n"
                          "function do_stuff[A](A a) : A\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/formal_type_parameter_with_the_same_name_as_polymorphic_compound.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/functions/formal_type_parameter_with_the_same_name_as_polymorphic_compound.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: false\n"
                       "        formal_type_parameter: T\n"
                       "        field\n"
                       "            type_reference: T\n"
                       "            name: x\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_type_parameter: A\n"
                       "        formal_parameter\n"
                       "            type_reference: A\n"
                       "            name: a\n"
                       "        return_type_reference: A\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void matching_functions_in_different_files_in_the_same_namespace(void)
{
    format_test_name("semantics/functions/matching_functions_in_different_files_in_the_same_namespace");

    format_source_path(0, "semantics/functions/matching_functions_in_different_files_in_the_same_namespace-1.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    format_source_path(1, "semantics/functions/matching_functions_in_different_files_in_the_same_namespace-2.owen");
    format_source_file(1, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/matching_functions_in_different_files_in_the_same_namespace-1.owen "
                     "semantics/functions/matching_functions_in_different_files_in_the_same_namespace-2.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/functions/matching_functions_in_different_files_in_the_same_namespace-1.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n"
                       "file\n"
                       "    path: semantics/functions/matching_functions_in_different_files_in_the_same_namespace-2.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n");
}

static void matching_monomorphic_functions(void)
{
    format_test_name("semantics/functions/matching_monomorphic_functions");

    format_source_path(0, "semantics/functions/matching_monomorphic_functions.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function do_stuff(I32 a) : I32\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function do_stuff(I32 a) : I32\n"
                          "    return a\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/matching_monomorphic_functions.owen");
    format_expectation(&test.expected_error, 
                       "semantics/functions/matching_monomorphic_functions.owen:4:10: Matching declarations for do_stuff in scope.\n"
                       "Declarations:\n"
                       "    semantics/functions/matching_monomorphic_functions.owen:4:10: do_stuff(I32 a) : I32\n"
                       "    semantics/functions/matching_monomorphic_functions.owen:8:10: do_stuff(I32 a) : I32\n");
}

static void matching_polymorphic_functions(void)
{
    format_test_name("semantics/functions/matching_polymorphic_functions");

    format_source_path(0, "semantics/functions/matching_polymorphic_functions.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function do_stuff[T1](T1 a) : T1\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function do_stuff[T2](T2 b) : T2\n"
                          "    return b\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/matching_polymorphic_functions.owen");
    format_expectation(&test.expected_error, 
                       "semantics/functions/matching_polymorphic_functions.owen:4:10: Matching declarations for do_stuff in scope.\n"
                       "Declarations:\n"
                       "    semantics/functions/matching_polymorphic_functions.owen:4:10: do_stuff[T1](T1 a) : T1\n"
                       "    semantics/functions/matching_polymorphic_functions.owen:8:10: do_stuff[T2](T2 b) : T2\n");
}

static void matching_qualified_and_unqualified_nth_formal_parameters(void)
{
    format_test_name("semantics/functions/matching_qualified_and_unqualified_nth_formal_parameters");

    format_source_path(0, "semantics/functions/matching_qualified_and_unqualified_nth_formal_parameters.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(#I32 b)\n"
                          "end\n"
                          "\n"
                          "function a(noalias #I32 b)\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/matching_qualified_and_unqualified_nth_formal_parameters.owen");
    format_expectation(&test.expected_error, 
                       "semantics/functions/matching_qualified_and_unqualified_nth_formal_parameters.owen:4:10: Matching declarations for a in scope.\n"
                       "Declarations:\n"
                       "    semantics/functions/matching_qualified_and_unqualified_nth_formal_parameters.owen:4:10: a(#I32 b)\n"
                       "    semantics/functions/matching_qualified_and_unqualified_nth_formal_parameters.owen:7:10: a(noalias #I32 b)\n");
}

static void matching_public_functions_in_different_files_in_the_same_namespace(void)
{
    format_test_name("semantics/functions/matching_public_functions_in_different_files_in_the_same_namespace");

    format_source_path(0, "semantics/functions/matching_public_functions_in_different_files_in_the_same_namespace-1.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "public function a()\n"
                          "end");

    format_source_path(1, "semantics/functions/matching_public_functions_in_different_files_in_the_same_namespace-2.owen");
    format_source_file(1, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "public function a()\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/matching_public_functions_in_different_files_in_the_same_namespace-1.owen "
                     "semantics/functions/matching_public_functions_in_different_files_in_the_same_namespace-2.owen "
                     "-print-semantics");

    format_expectation(&test.expected_error, 
                       "semantics/functions/matching_public_functions_in_different_files_in_the_same_namespace-1.owen:4:17: Matching declarations for a in scope.\n"
                       "Declarations:\n"
                       "    semantics/functions/matching_public_functions_in_different_files_in_the_same_namespace-1.owen:4:17: a()\n"
                       "    semantics/functions/matching_public_functions_in_different_files_in_the_same_namespace-2.owen:4:17: a()\n");
}

static void matching_public_functions_in_the_same_file(void)
{
    format_test_name("semantics/functions/matching_public_functions_in_the_same_file");

    format_source_path(0, "semantics/functions/matching_public_functions_in_the_same_file.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "public function a()\n"
                          "end\n"
                          "\n"
                          "public function a()\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/matching_public_functions_in_the_same_file.owen");
    format_expectation(&test.expected_error, 
                       "semantics/functions/matching_public_functions_in_the_same_file.owen:4:17: Matching declarations for a in scope.\n"
                       "Declarations:\n"
                       "    semantics/functions/matching_public_functions_in_the_same_file.owen:4:17: a()\n"
                       "    semantics/functions/matching_public_functions_in_the_same_file.owen:7:17: a()\n");
}

static void nested_formal_type_parameter_in_formal_parameter(void)
{
    format_test_name("semantics/functions/nested_formal_type_parameter_in_formal_parameter");

    format_source_path(0, "semantics/functions/nested_formal_type_parameter_in_formal_parameter.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T](T[I32] b)\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/nested_formal_type_parameter_in_formal_parameter.owen");
    format_expectation(&test.expected_error, 
                       "%s:4:15: Type parameters can't be nested.\n",
                       test.source_paths[0]);
}

static void nested_formal_type_parameter_in_return_type(void)
{
    format_test_name("semantics/functions/nested_formal_type_parameter_in_return_type");

    format_source_path(0, "semantics/functions/nested_formal_type_parameter_in_return_type.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T]() : T[I32]\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/nested_formal_type_parameter_in_return_type.owen");
    format_expectation(&test.expected_error, 
                       "%s:4:19: Type parameters can't be nested.\n",
                       test.source_paths[0]);
}

static void non_matching_functions_with_the_same_name(void)
{
    format_test_name("semantics/functions/non_matching_functions_with_the_same_name");

    format_source_path(0, "semantics/functions/non_matching_functions_with_the_same_name.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure Structure[T1, T2]\n"
                          "    T1 a\n"
                          "    T2 b\n"
                          "end\n"
                          "\n"
                          "structure Structure[T]\n"
                          "    T a\n"
                          "end\n"
                          "\n"
                          "function do_stuff()\n"
                          "end\n"
                          "\n"
                          "function do_stuff(I32 a) : I32\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function do_stuff(F32 a) : F32\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function do_stuff(F32 a, I32 b) : F32, I32\n"
                          "    return a, b\n"
                          "end\n"
                          "\n"
                          "function do_stuff(I32 a, F32 b) : I32, F32\n"
                          "    return a, b\n"
                          "end\n"
                          "\n"
                          "function do_stuff(#I32 a) : #I32\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function do_stuff(#F32 a) : #F32\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function do_stuff([]I32 a) : []I32\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function do_stuff([2]I32 a) : [2]I32\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function do_stuff([3]I32 a) : [3]I32\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function do_stuff[T]([]T a) : []T\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function do_stuff[T]([2]T a) : [2]T\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function do_stuff[T]([3]T a) : [3]T\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function do_stuff(Structure[I32] a) : Structure[I32]\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function do_stuff(Structure[I32, I32] a) : Structure[I32, I32]\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function do_stuff[T](Structure[T] a) : Structure[T]\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function do_stuff[T1, T2](Structure[T1, T2] a) : Structure[T1, T2]\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function do_stuff[T](T a) : T\n"
                          "    return a\n"
                          "end\n"
                          "\n"
                          "function do_stuff[T](T a, T b) : T\n"
                          "    return a + b\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/non_matching_functions_with_the_same_name.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/functions/non_matching_functions_with_the_same_name.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: Structure\n"
                       "        is_public: false\n"
                       "        formal_type_parameter: T1\n"
                       "        formal_type_parameter: T2\n"
                       "        field\n"
                       "            type_reference: T1\n"
                       "            name: a\n"
                       "        field\n"
                       "            type_reference: T2\n"
                       "            name: b\n"
                       "    structure\n"
                       "        name: Structure\n"
                       "        is_public: false\n"
                       "        actual_type_parameter: I32\n"
                       "        actual_type_parameter: I32\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: a\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: b\n"
                       "    structure\n"
                       "        name: Structure\n"
                       "        is_public: false\n"
                       "        formal_type_parameter: T\n"
                       "        field\n"
                       "            type_reference: T\n"
                       "            name: a\n"
                       "    structure\n"
                       "        name: Structure\n"
                       "        is_public: false\n"
                       "        actual_type_parameter: I32\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: a\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: a\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_parameter\n"
                       "            type: F32\n"
                       "            name: a\n"
                       "        return_type: F32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: F32\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_parameter\n"
                       "            type: F32\n"
                       "            name: a\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: b\n"
                       "        return_type: F32\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: F32\n"
                       "                name: a\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_parameter\n"
                       "            type: I32\n"
                       "            name: a\n"
                       "        formal_parameter\n"
                       "            type: F32\n"
                       "            name: b\n"
                       "        return_type: I32\n"
                       "        return_type: F32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: I32\n"
                       "                name: a\n"
                       "            reference_expression\n"
                       "                type: F32\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_parameter\n"
                       "            type: #I32\n"
                       "            name: a\n"
                       "        return_type: #I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: #I32\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_parameter\n"
                       "            type: #F32\n"
                       "            name: a\n"
                       "        return_type: #F32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: #F32\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: a\n"
                       "        return_type: []I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: []I32\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_parameter\n"
                       "            type: [2]I32\n"
                       "            name: a\n"
                       "        return_type: [2]I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: [2]I32\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_parameter\n"
                       "            type: [3]I32\n"
                       "            name: a\n"
                       "        return_type: [3]I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: [3]I32\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: []T\n"
                       "            name: a\n"
                       "        return_type_reference: []T\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: [2]T\n"
                       "            name: a\n"
                       "        return_type_reference: [2]T\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: [3]T\n"
                       "            name: a\n"
                       "        return_type_reference: [3]T\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_parameter\n"
                       "            type: Structure[I32]\n"
                       "            name: a\n"
                       "        return_type: Structure[I32]\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: Structure[I32]\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_parameter\n"
                       "            type: Structure[I32, I32]\n"
                       "            name: a\n"
                       "        return_type: Structure[I32, I32]\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: Structure[I32, I32]\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: Structure[T]\n"
                       "            name: a\n"
                       "        return_type_reference: Structure[T]\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_type_parameter: T1\n"
                       "        formal_type_parameter: T2\n"
                       "        formal_parameter\n"
                       "            type_reference: Structure[T1, T2]\n"
                       "            name: a\n"
                       "        return_type_reference: Structure[T1, T2]\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: T\n"
                       "            name: a\n"
                       "        return_type_reference: T\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: T\n"
                       "            name: a\n"
                       "        formal_parameter\n"
                       "            type_reference: T\n"
                       "            name: b\n"
                       "        return_type_reference: T\n"
                       "        return_statement\n"
                       "            binary\n"
                       "                operator: +\n"
                       "                reference_expression\n"
                       "                    name: a\n"
                       "                reference_expression\n"
                       "                    name: b\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void public_function_returning_non_public_compound(void)
{
    format_test_name("semantics/functions/public_function_returning_non_public_compound");

    format_source_path(0, "semantics/functions/public_function_returning_non_public_compound.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    I32 a\n"
                          "end\n"
                          "\n"
                          "public function a() : A\n"
                          "    return { a = 0 }\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/public_function_returning_non_public_compound.owen");
    format_expectation(&test.expected_error, 
                       "%s:8:17: a exposes A which is not public.\n",
                       test.source_paths[0]);
}

static void public_function_returning_non_public_enumeration(void)
{
    format_test_name("semantics/functions/public_function_returning_non_public_enumeration");

    format_source_path(0, "semantics/functions/public_function_returning_non_public_enumeration.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : I32\n"
                          "    a\n"
                          "end\n"
                          "\n"
                          "public function a() : A\n"
                          "    return A.a\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/public_function_returning_non_public_enumeration.owen");
    format_expectation(&test.expected_error, 
                       "%s:8:17: a exposes A which is not public.\n",
                       test.source_paths[0]);
}

static void public_function_returning_public_compound(void)
{
    format_test_name("semantics/functions/public_function_returning_public_compound");

    format_source_path(0, "semantics/functions/public_function_returning_public_compound.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "public structure A\n"
                          "    I32 a\n"
                          "end\n"
                          "\n"
                          "public function a() : A\n"
                          "    return { a = 1 }\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/public_function_returning_public_compound.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/functions/public_function_returning_public_compound.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: true\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: a\n"
                       "    function\n"
                       "        name: a\n"
                       "        return_type: A\n"
                       "        return_statement\n"
                       "            compound_literal\n"
                       "                type: A\n"
                       "                field_initializer\n"
                       "                    type: I32\n"
                       "                    name: a\n"
                       "                    integer_literal\n"
                       "                        type: I32\n"
                       "                        value: 1\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void public_function_returning_public_enumeration(void)
{
    format_test_name("semantics/functions/public_function_returning_public_enumeration");

    format_source_path(0, "semantics/functions/public_function_returning_public_enumeration.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "public enumeration A : I32\n"
                          "    a\n"
                          "end\n"
                          "\n"
                          "public function a() : A\n"
                          "    return A.a\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/public_function_returning_public_enumeration.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/functions/public_function_returning_public_enumeration.owen\n"
                       "    namespace: Abc\n"
                       "    enumeration\n"
                       "        name: A\n"
                       "        underlying_type: I32\n"
                       "        is_public: true\n"
                       "        constant\n"
                       "            name: a\n"
                       "            value: 0\n"
                       "    function\n"
                       "        name: a\n"
                       "        return_type: A\n"
                       "        return_statement\n"
                       "            enumeration_constant_access\n"
                       "                type: A\n"
                       "                value: a\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void public_function_with_formal_parameter_with_non_public_compound(void)
{
    format_test_name("semantics/functions/public_function_with_formal_parameter_with_non_public_compound");

    format_source_path(0, "semantics/functions/public_function_with_formal_parameter_with_non_public_compound.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "structure A\n"
                          "    I32 x\n"
                          "end\n"
                          "\n"
                          "public function a(A b) : A\n"
                          "    return b\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/public_function_with_formal_parameter_with_non_public_compound.owen");
    format_expectation(&test.expected_error, 
                       "%s:8:17: a exposes A which is not public.\n",
                       test.source_paths[0]);
}

static void public_function_with_formal_parameter_with_non_public_enumeration(void)
{
    format_test_name("semantics/functions/public_function_with_formal_parameter_with_non_public_enumeration");

    format_source_path(0, "semantics/functions/public_function_with_formal_parameter_with_non_public_enumeration.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "enumeration A : I32\n"
                          "    a\n"
                          "end\n"
                          "\n"
                          "public function a(A b) : A\n"
                          "    return b\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/public_function_with_formal_parameter_with_non_public_enumeration.owen");
    format_expectation(&test.expected_error, 
                       "%s:8:17: a exposes A which is not public.\n",
                       test.source_paths[0]);
}

static void public_function_with_formal_parameter_with_public_compound(void)
{
    format_test_name("semantics/functions/public_function_with_formal_parameter_with_public_compound");

    format_source_path(0, "semantics/functions/public_function_with_formal_parameter_with_public_compound.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "public structure A\n"
                          "    I32 a\n"
                          "end\n"
                          "\n"
                          "public function do_stuff(A a) : A\n"
                          "    return a\n"
                          "end"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/public_function_with_formal_parameter_with_public_compound.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/functions/public_function_with_formal_parameter_with_public_compound.owen\n"
                       "    namespace: Abc\n"
                       "    structure\n"
                       "        name: A\n"
                       "        is_public: true\n"
                       "        field\n"
                       "            type: I32\n"
                       "            name: a\n"
                       "    function\n"
                       "        name: do_stuff\n"
                       "        formal_parameter\n"
                       "            type: A\n"
                       "            name: a\n"
                       "        return_type: A\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: A\n"
                       "                name: a\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void public_function_with_formal_parameter_with_public_enumeration(void)
{
    format_test_name("semantics/functions/public_function_with_formal_parameter_with_public_enumeration");

    format_source_path(0, "semantics/functions/public_function_with_formal_parameter_with_public_enumeration.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "public enumeration A : I32\n"
                          "    a\n"
                          "end\n"
                          "\n"
                          "public function a(A b) : A\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/public_function_with_formal_parameter_with_public_enumeration.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/functions/public_function_with_formal_parameter_with_public_enumeration.owen\n"
                       "    namespace: Abc\n"
                       "    enumeration\n"
                       "        name: A\n"
                       "        underlying_type: I32\n"
                       "        is_public: true\n"
                       "        constant\n"
                       "            name: a\n"
                       "            value: 0\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_parameter\n"
                       "            type: A\n"
                       "            name: b\n"
                       "        return_type: A\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: A\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void unreferenced_formal_parameter(void)
{
    format_test_name("semantics/functions/unreferenced_formal_parameter");

    format_source_path(0, "semantics/functions/unreferenced_formal_parameter.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a(I32 b)\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/unreferenced_formal_parameter.owen");
    format_expectation(&test.expected_error, 
                       "%s:4:16: Unused variable.\n",
                       test.source_paths[0]);
}

static void would_be_matching_functions_but_in_different_namespaces(void)
{
    format_test_name("semantics/functions/would_be_matching_functions_but_in_different_namespaces");

    format_source_path(0, "semantics/functions/would_be_matching_functions_but_in_different_namespaces-1.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a()\n"
                          "end"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    format_source_path(1, "semantics/functions/would_be_matching_functions_but_in_different_namespaces-2.owen");
    format_source_file(1, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Def\n"
                          "\n"
                          "function a()\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/would_be_matching_functions_but_in_different_namespaces-1.owen "
                     "semantics/functions/would_be_matching_functions_but_in_different_namespaces-2.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/functions/would_be_matching_functions_but_in_different_namespaces-1.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n"
                       "file\n"
                       "    path: semantics/functions/would_be_matching_functions_but_in_different_namespaces-2.owen\n"
                       "    namespace: Def\n"
                       "    function\n"
                       "        name: a\n");
}

static void pointer_to_formal_type_parameter(void)
{
    format_test_name("semantics/functions/pointer_to_formal_type_parameter");

    format_source_path(0, "semantics/functions/pointer_to_formal_type_parameter.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T](#T b) : #T\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function c(#I32 b) : #I32\n"
                          "    return a(b)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/pointer_to_formal_type_parameter.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/functions/pointer_to_formal_type_parameter.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: #T\n"
                       "            name: b\n"
                       "        return_type_reference: #T\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: #I32\n"
                       "            name: b\n"
                       "        return_type: #I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: #I32\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: #I32\n"
                       "            name: b\n"
                       "        return_type: #I32\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: #I32\n"
                       "                reference_expression\n"
                       "                    type: Function(#I32) : #I32\n"
                       "                    name: a\n"
                       "                reference_expression\n"
                       "                    type: #I32\n"
                       "                    name: b\n"
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
    format_test_name("semantics/functions/dynamic_array_of_formal_type_parameter");

    format_source_path(0, "semantics/functions/dynamic_array_of_formal_type_parameter.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T]([]T b) : []T\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function c([]I32 b) : []I32\n"
                          "    return a(b)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/dynamic_array_of_formal_type_parameter.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/functions/dynamic_array_of_formal_type_parameter.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: []T\n"
                       "            name: b\n"
                       "        return_type_reference: []T\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: b\n"
                       "        return_type: []I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: []I32\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: []I32\n"
                       "            name: b\n"
                       "        return_type: []I32\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: []I32\n"
                       "                reference_expression\n"
                       "                    type: Function([]I32) : []I32\n"
                       "                    name: a\n"
                       "                reference_expression\n"
                       "                    type: []I32\n"
                       "                    name: b\n"
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
    format_test_name("semantics/functions/fixed_array_of_formal_type_parameter");

    format_source_path(0, "semantics/functions/fixed_array_of_formal_type_parameter.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function a[T]([2]T b) : [2]T\n"
                          "    return b\n"
                          "end\n"
                          "\n"
                          "function c([2]I32 b) : [2]I32\n"
                          "    return a(b)\n"
                          "end\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/fixed_array_of_formal_type_parameter.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/functions/fixed_array_of_formal_type_parameter.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: a\n"
                       "        formal_type_parameter: T\n"
                       "        formal_parameter\n"
                       "            type_reference: [2]T\n"
                       "            name: b\n"
                       "        return_type_reference: [2]T\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: a\n"
                       "        actual_type_parameter: I32\n"
                       "        formal_parameter\n"
                       "            type: [2]I32\n"
                       "            name: b\n"
                       "        return_type: [2]I32\n"
                       "        return_statement\n"
                       "            reference_expression\n"
                       "                type: [2]I32\n"
                       "                name: b\n"
                       "    function\n"
                       "        name: c\n"
                       "        formal_parameter\n"
                       "            type: [2]I32\n"
                       "            name: b\n"
                       "        return_type: [2]I32\n"
                       "        return_statement\n"
                       "            call_expression\n"
                       "                type: [2]I32\n"
                       "                reference_expression\n"
                       "                    type: Function([2]I32) : [2]I32\n"
                       "                    name: a\n"
                       "                reference_expression\n"
                       "                    type: [2]I32\n"
                       "                    name: b\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void single_main_function(void)
{
    format_test_name("semantics/functions/single_main_function");

    format_source_path(0, "semantics/functions/single_main_function.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end");

    SET_COMMAND_LINE("semantics/functions/single_main_function.owen "
                     "-print-semantics");

    format_expectation(&test.expected_semantics, 
                       "file\n"
                       "    path: semantics/functions/single_main_function.owen\n"
                       "    namespace: Abc\n"
                       "    function\n"
                       "        name: main\n"
                       "        return_type: I32\n"
                       "        return_statement\n"
                       "            integer_literal\n"
                       "                type: I32\n"
                       "                value: 0\n");
}

static void multiple_main_functions(void)
{
    format_test_name("semantics/functions/multiple_main_functions");

    format_source_path(0, "semantics/functions/multiple_main_functions.owen");
    format_source_file(0, "// Generated at " __FILE__ ":" LINE_STRING "\n"
                          "namespace Abc\n"
                          "\n"
                          "function main() : I32\n"
                          "    return 0\n"
                          "end\n"
                          "\n"
                          "function main(#[][]U8 args) : I32\n"
                          "    return 0\n"
                          "end\n");

    SET_COMMAND_LINE("semantics/functions/multiple_main_functions.owen");
    format_expectation(&test.expected_error, "%s:8:10: main function redeclared.\n", test.source_paths[0]);
}

bool semantics_functions(void)
{
    static uint8_t state;
    switch (state)
    {
        case 0:
            duplicate_formal_type_parameters();
            state++;
            break;

        case 1:
            formal_parameter_with_the_same_name_as_a_function();
            state++;
            break;

        case 2:
            formal_type_parameter_with_the_same_name_as_a_primitive();
            state++;
            break;

        case 3:
            formal_type_parameter_with_the_same_name_as_an_enumeration();
            state++;
            break;

        case 4:
            formal_type_parameter_with_the_same_name_as_polymorphic_compound();
            state++;

            break;

        case 5:
            matching_functions_in_different_files_in_the_same_namespace();
            state++;
            break;

        case 6:
            matching_monomorphic_functions();
            state++;
            break;

        case 7:
            matching_polymorphic_functions();
            state++;
            break;

        case 8:
            matching_qualified_and_unqualified_nth_formal_parameters();
            state++;
            break;

        case 9:
            matching_public_functions_in_different_files_in_the_same_namespace();
            state++;
            break;

        case 10:
            matching_public_functions_in_the_same_file();
            state++;
            break;

        case 11:
            nested_formal_type_parameter_in_formal_parameter();
            state++;
            break;

        case 12:
            nested_formal_type_parameter_in_return_type();
            state++;
            break;

        case 13:
            non_matching_functions_with_the_same_name();
            state++;
            break;

        case 14:
            public_function_returning_non_public_compound();
            state++;
            break;

        case 15:
            public_function_returning_non_public_enumeration();
            state++;
            break;

        case 16:
            public_function_returning_public_compound();
            state++;
            break;

        case 17:
            public_function_returning_public_enumeration();
            state++;
            break;

        case 18:
            public_function_with_formal_parameter_with_non_public_compound();
            state++;
            break;

        case 19:
            public_function_with_formal_parameter_with_non_public_enumeration();
            state++;
            break;

        case 20:
            public_function_with_formal_parameter_with_public_compound();
            state++;
            break;

        case 21:
            public_function_with_formal_parameter_with_public_enumeration();
            state++;
            break;

        case 22:
            unreferenced_formal_parameter();
            state++;
            break;

        case 23:
            would_be_matching_functions_but_in_different_namespaces();
            state++;
            break;

        case 24:
            pointer_to_formal_type_parameter();
            state++;
            break;

        case 25:
            dynamic_array_of_formal_type_parameter();
            state++;
            break;

        case 26:
            fixed_array_of_formal_type_parameter();
            state++;
            break;

        case 27:
            single_main_function();
            state++;
            break;

        case 28:
            multiple_main_functions();
            state++;
            break;

        default:
            return false;
    }

    return true;
}
