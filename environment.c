#include "environment.h"
#include "interpreter.h" // Include for Value definition
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Safe local strdup replacement to avoid platform-specific issues
static char* my_strdup_local(const char* s) {
    if (s == NULL) return NULL;
    size_t len = strlen(s) + 1;
    char* d = (char*)malloc(len);
    if (!d) return NULL;
    memcpy(d, s, len);
    return d;
}

Environment* create_environment() {
    Environment* env = (Environment*)malloc(sizeof(Environment));
    if (env == NULL) {
        perror("Failed to allocate environment");
        exit(1);
    }
    env->parent = NULL;
    env->variables = NULL;
    env->functions = NULL;
    return env;
}

Environment* create_child_environment(Environment* parent) {
    Environment* env = create_environment();
    env->parent = parent;
    return env;
}

void destroy_environment(Environment* env) {
    Symbol* current_var = env->variables;
    while (current_var != NULL) {
        Symbol* next_var = current_var->next;
        free(current_var->name);
        free(current_var);
        current_var = next_var;
    }

    FunctionSymbol* current_func = env->functions;
    while (current_func != NULL) {
        FunctionSymbol* next_func = current_func->next;
        free(current_func->name);
        // The Function* itself might be freed elsewhere if it's shared,
        // or it might be part of the AST which is freed separately.
        // For now, we assume the Function* is not owned by the FunctionSymbol.
        free(current_func);
        current_func = next_func;
    }
    free(env);
}

void env_add_variable(Environment* env, const char* name, Value value) {
    Symbol* new_symbol = (Symbol*)malloc(sizeof(Symbol));
    if (new_symbol == NULL) {
        perror("Failed to allocate symbol");
        exit(1);
    }
    new_symbol->name = my_strdup_local(name);
    if (new_symbol->name == NULL) {
        perror("Failed to duplicate symbol name");
        exit(1);
    }
    new_symbol->value = value;
    new_symbol->next = env->variables;
    env->variables = new_symbol;
}

Value env_get_variable(Environment* env, const char* name) {
    Environment* current_env = env;
    while (current_env != NULL) {
        Symbol* current_var = current_env->variables;
        while (current_var != NULL) {
            if (strcmp(current_var->name, name) == 0) {
                return current_var->value;
            }
            current_var = current_var->next;
        }
        current_env = current_env->parent;
    }
    // Handle error: variable not found
    fprintf(stderr, "Runtime Error: Undefined variable '%s'\n", name);
    exit(1);
}

void env_set_variable(Environment* env, const char* name, Value value) {
    Environment* current_env = env;
    while (current_env != NULL) {
        Symbol* current_var = current_env->variables;
        while (current_var != NULL) {
            if (strcmp(current_var->name, name) == 0) {
                current_var->value = value;
                return;
            }
            current_var = current_var->next;
        }
        current_env = current_env->parent;
    }
    // Variable not found, add it to current environment
    env_add_variable(env, name, value);
}

void env_add_function(Environment* env, const char* name, Function* func) {
    FunctionSymbol* new_func_symbol = (FunctionSymbol*)malloc(sizeof(FunctionSymbol));
    if (new_func_symbol == NULL) {
        perror("Failed to allocate function symbol");
        exit(1);
    }
    new_func_symbol->name = my_strdup_local(name);
    if (new_func_symbol->name == NULL) {
        perror("Failed to duplicate function symbol name");
        exit(1);
    }
    new_func_symbol->func = func;
    new_func_symbol->next = env->functions;
    env->functions = new_func_symbol;
}

Function* env_get_function(Environment* env, const char* name) {
    Environment* current_env = env;
    while (current_env != NULL) {
        FunctionSymbol* current_func_symbol = current_env->functions;
        while (current_func_symbol != NULL) {
            if (strcmp(current_func_symbol->name, name) == 0) {
                return current_func_symbol->func;
            }
            current_func_symbol = current_func_symbol->next;
        }
        current_env = current_env->parent;
    }
    return NULL; // Function not found
}