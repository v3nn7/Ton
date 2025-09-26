#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include "value.h"

// Forward declaration for Environment to avoid circular dependency
typedef struct Environment Environment;

Value interpret_ast(ASTNode* node, Environment* env);

extern int program_exit_code;

#endif // INTERPRETER_H