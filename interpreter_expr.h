#ifndef INTERPRETER_EXPR_H
#define INTERPRETER_EXPR_H

#include "ast.h"
#include "environment.h"
#include "value.h"
#include "error.h"

TonError interpret_expression(ASTNode* node, Environment* env, Value* out_result);

#endif // INTERPRETER_EXPR_H