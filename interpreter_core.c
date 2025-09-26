#include "interpreter_core.h"
#include "io.h"
#include "bitops.h"
#include "array.h"
#include "struct.h"
#include "module.h"
#include "error.h"
#include "builtin.h"

Value eval_expression(ASTNode* expr, Environment* env) {
    (void)expr; (void)env;
    return create_value_int(0);
}

Value exec_statement(ASTNode* stmt, Environment* env) {
    (void)stmt; (void)env;
    return create_value_int(0);
}

Value exec_program(ASTNode* program, Environment* env) {
    (void)program; (void)env;
    return create_value_int(0);
}

void register_builtins(Environment* env) {
    // Delegate to builtin module to install built-in symbols into the environment
    install_builtins(env);
}