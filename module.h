#ifndef TON_MODULE_H
#define TON_MODULE_H

#include "environment.h"
#include "interpreter.h"

typedef struct TonModule {
    const char* name;
    Environment* env;
} TonModule;

TonModule* load_module(const char* path, Environment* parent_env);
void       unload_module(TonModule* m);
int        import_symbol(TonModule* m, const char* symbol_name, Environment* target_env);

#endif // TON_MODULE_H