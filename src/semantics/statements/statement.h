#pragma once

#include "../semantics.h"

void type_check_declaration_statement(const File* file, Declaration_Statement* declaration_statement);
void type_check_assignment_statement(const File* file, Assignment_Statement* assignment_statement);
void type_check_expression_statement(const File* file, Expression* expression_statement);
bool type_check_if_statement(const File* file, const If_Statement* if_statement, const Function* function, bool is_inside_loop);
void type_check_for_statement(const File* file, For_Statement* for_statement, const Function* function);
void type_check_while_statement(const File* file, While_Statement* while_statement, const Function* function);
void type_check_break_statement(const File* file, const Break_Statement* break_statement, bool is_inside_loop);
void type_check_continue_statement(const File* file, const Continue_Statement* break_statement, bool is_inside_loop);
void type_check_return_statement(const File* file, const Return_Statement* return_statement, const Function* function);

void deep_copy_declaration_statement(Declaration_Statement* restrict destination, const Declaration_Statement* restrict source);
void deep_copy_assignment_statement(Assignment_Statement* restrict destination, const Assignment_Statement* restrict source);
void deep_copy_expression_statement(Call* restrict destination, const Call* restrict source);
void deep_copy_if_statement(If_Statement* restrict destination, const If_Statement* restrict source);
void deep_copy_for_statement(For_Statement* restrict destination, const For_Statement* restrict source);
void deep_copy_while_statement(While_Statement* restrict destination, const While_Statement* restrict source);
void deep_copy_break_statement(Break_Statement* restrict destination, const Break_Statement* restrict source);
void deep_copy_continue_statement(Continue_Statement* restrict destination, const Continue_Statement* restrict source);
void deep_copy_return_statement(Return_Statement* restrict destination, const Return_Statement* restrict source);
