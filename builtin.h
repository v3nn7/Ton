#ifndef TON_BUILTIN_H
#define TON_BUILTIN_H

#include "interpreter.h"
#include "environment.h"

void install_builtins(Environment* env);
Function* make_builtin_fn(const char* name); // Dodano deklarację

#endif // TON_BUILTIN_H