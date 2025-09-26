#ifndef TON_INTERPRETER_CORE_H
#define TON_INTERPRETER_CORE_H

#include "ast.h"
#include "environment.h"
#include "interpreter_stmt.h"
#include "interpreter_expr.h"

void register_builtins(Environment* env);

#endif // TON_INTERPRETER_CORE_H