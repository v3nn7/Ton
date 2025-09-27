#include "builtin.h"
#include "builtin_tonlib.h"
#include "builtin_crypto.h"
#include "io.h"
#include "bitops.h"
#include "array.h"
#include "struct.h"
#include "memory.h"
#include "tonlib_low.h"
#include <string.h>
#include <stdlib.h>

// For now, our interpreter.c handles built-ins by name in call dispatch.
// We still register stub Function entries so that env_get_function can find them
// and future refactors can move implementation behind these symbols.
Function* make_builtin_fn(const char* name) {
    Function* f = (Function*)ton_malloc(sizeof(Function));
    if (!f) return NULL;
    f->type = BUILT_IN;     // Set the type to BUILT_IN
    f->name = ton_strdup(name);
    f->body = NULL;         // No AST body for builtins
    f->closure_env = NULL;  // Not used for builtins currently
    return f;
}

void install_builtins(Environment* env) {
    if (!env) return;

    // Install TonLib built-in functions
    install_tonlib_builtins(env);

    // Install Crypto built-in functions
    install_crypto_builtins(env);

    // Install TonLib Low-level built-in functions
    register_tonlib_low_functions(env);

    // IO-like builtins that are treated as identifiers
    env_add_function(env, "read_line", make_builtin_fn("read_line"));

    // Bitwise operations
    env_add_function(env, "bit_and", make_builtin_fn("bit_and"));
    env_add_function(env, "bit_or",  make_builtin_fn("bit_or"));
    env_add_function(env, "bit_xor", make_builtin_fn("bit_xor"));
    env_add_function(env, "bit_not", make_builtin_fn("bit_not"));
    env_add_function(env, "bit_shl", make_builtin_fn("bit_shl"));
    env_add_function(env, "bit_shr", make_builtin_fn("bit_shr"));

    // In the future: memory and arrays (placeholders for API surface)
    // env_add_function(env, "malloc",    make_builtin_fn("malloc"));
    // env_add_function(env, "free",      make_builtin_fn("free"));
    // env_add_function(env, "array_push",make_builtin_fn("array_push"));
}