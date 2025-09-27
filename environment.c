#include "error.h"
#include "environment.h"
#include "interpreter.h" // Include for Value definition
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



Environment* create_environment() {
    Environment* env = (Environment*)ton_malloc(sizeof(Environment));
    if (env == NULL) {
        runtime_error("Failed to allocate environment");
        return NULL;
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

void env_add_ref(Environment* env) {
    if (env) {
        env->ref_count++;
    }
}

void env_release(Environment* env) {
    if (!env) return;

    env->ref_count--;
    if (env->ref_count > 0) {
        return;
    }

    Symbol* current = env->variables;
     while (current) {
         Symbol* next = current->next;
        ton_free(current->name);
        value_release(&current->value); // Release the value
        ton_free(current);
        current = next;
    }

    FunctionSymbol* current_func = env->functions;
    while (current_func != NULL) {
        FunctionSymbol* next_func = current_func->next;
        ton_free(current_func->name);
        // The Function* itself might be freed elsewhere if it's shared,
        // or it might be part of the AST which is freed separately.
        // For now, we assume the Function* is not owned by the FunctionSymbol.
        ton_free(current_func);
        current_func = next_func;
    }
    ton_free(env);
}

void env_add_variable(Environment* env, const char* name, Value value, VariableType type) {
    Symbol* new_symbol = (Symbol*)ton_malloc(sizeof(Symbol));
    if (new_symbol == NULL) {
        runtime_error("Failed to allocate symbol");
        return;
    }
    new_symbol->name = ton_strdup(name);
    if (new_symbol->name == NULL) {
        runtime_error("Failed to duplicate symbol name");
        ton_free(new_symbol);
        return;
    }
    new_symbol->value = value;
    new_symbol->type = type;
    new_symbol->next = env->variables;
    env->variables = new_symbol;
}

Value* env_get_variable(Environment* env, const char* name) {
    Environment* current_env = env;
    while (current_env != NULL) {
        Symbol* current_var = current_env->variables;
        while (current_var != NULL) {
            if (strcmp(current_var->name, name) == 0) {
                return &current_var->value;
            }
            current_var = current_var->next;
        }
        current_env = current_env->parent;
    }
    runtime_error("Undefined variable '%s'", name);
    return NULL; // Return NULL when variable is not found
}

bool env_set_variable(Environment* env, const char* name, Value value) {
    Environment* current_env = env;
    while (current_env != NULL) {
        Symbol* current_var = current_env->variables;
        while (current_var != NULL) {
            if (strcmp(current_var->name, name) == 0) {
                value_release(&current_var->value); // Release the old value
                current_var->value = value;
                value_add_ref(&current_var->value); // Add reference to the new value
                return true;
            }
            current_var = current_var->next;
        }
        current_env = current_env->parent;
    }
    return false; // Variable not found
}

void env_add_function(Environment* env, const char* name, Function* func) {
    FunctionSymbol* new_func_symbol = (FunctionSymbol*)ton_malloc(sizeof(FunctionSymbol));
    if (new_func_symbol == NULL) {
        runtime_error("Failed to allocate function symbol");
        return;
    }
    new_func_symbol->name = ton_strdup(name);
    if (new_func_symbol->name == NULL) {
        runtime_error("Failed to duplicate function symbol name");
        ton_free(new_func_symbol);
        return;
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