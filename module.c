#include "module.h"
#include <stdlib.h>

TonModule* load_module(const char* path, Environment* parent_env) {
    (void)path; (void)parent_env;
    return NULL; // TODO: zaimplementować ładowanie pliku .ton
}

void unload_module(TonModule* m) {
    (void)m;
}

int import_symbol(TonModule* m, const char* symbol_name, Environment* target_env) {
    (void)m; (void)symbol_name; (void)target_env;
    return 0;
}