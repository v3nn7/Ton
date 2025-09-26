#ifndef INTERPRETER_STMT_H
#define INTERPRETER_STMT_H

#include "ast.h"
#include "environment.h"
#include "value.h"
#include "error.h"

TonError interpret_statement(ASTNode* node, Environment* env, Value* out_result);

#endif // INTERPRETER_STMT_H