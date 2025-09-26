#ifndef TON_INTERPRETER_CORE_H
#define TON_INTERPRETER_CORE_H

#include "ast.h"
#include "environment.h"
#include "interpreter.h"

Value eval_expression(ASTNode* expr, Environment* env);
Value exec_statement(ASTNode* stmt, Environment* env);
Value exec_program(ASTNode* program, Environment* env);

void register_builtins(Environment* env);

#endif // TON_INTERPRETER_CORE_H