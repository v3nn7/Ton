#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "token.h"
#include "ast.h" // Include AST for Function definition
#include "value.h" // Include Value definition

// Simple symbol table for variables
typedef struct Symbol {
    char* name;
    Value value;  // Changed from int to Value
    VariableType type;
    struct Symbol* next;
} Symbol;

// Symbol table for functions
typedef struct FunctionSymbol {
    char* name;
    Function* func;
    struct FunctionSymbol* next;
} FunctionSymbol;

typedef struct Environment {
    struct Environment* parent; // For scope chaining
    Symbol* variables;
    FunctionSymbol* functions;
    int ref_count; // Add reference count
} Environment;

// Function structure (moved from interpreter.h)
struct Function {
    FunctionType type;
    char* name;
    ASTNode* body;
    Environment* closure_env; // Environment where the function was declared
    // Added for user-defined functions argument mapping
    ParameterNode** parameters;
    int num_parameters;
    VariableType return_type;
};

Environment* create_environment();
Environment* create_child_environment(Environment* parent);
void destroy_environment(Environment* env);
void env_add_variable(Environment* env, const char* name, Value value, VariableType type);
Value* env_get_variable(Environment* env, const char* name);
bool env_set_variable(Environment* env, const char* name, Value value);

// Function handling
void env_add_function(Environment* env, const char* name, Function* func);
Function* env_get_function(Environment* env, const char* name);
void env_add_ref(Environment* env);
void env_release(Environment* env);

#endif // ENVIRONMENT_H