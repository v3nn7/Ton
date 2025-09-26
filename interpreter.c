/*
 * Ton Interpreter
 * Version: 0.0.1
 * Author: v3nn7
 * License: Apache License 2.0
 *
 * This file is part of the Ton programming language interpreter.
 * For full license text, see the LICENSE file in the project root.
 */
#include "interpreter.h"
#include "environment.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitops.h"
#include "builtin_tonlib.h"
#include "array.h"
#include "struct.h"

// Dynamically read a line from stdin, trimming trailing \n and optional \r (Windows).
// Returns a malloc-allocated string (may be empty string), or NULL on allocation failure.
static char* read_line_alloc(void) {
    size_t cap = 128;
    size_t len = 0;
    char* buf = (char*)malloc(cap);
    if (!buf) return NULL;

    int ch;
    while ((ch = fgetc(stdin)) != EOF) {
        if (ch == '\n') break;
        if (len + 1 >= cap) {
            size_t new_cap = cap * 2;
            char* tmp = (char*)realloc(buf, new_cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
            cap = new_cap;
        }
        buf[len++] = (char)ch;
    }

    // Ensure space for terminator
    if (len + 1 >= cap) {
        char* tmp = (char*)realloc(buf, len + 1);
        if (!tmp) { free(buf); return NULL; }
        buf = tmp;
        cap = len + 1;
    }

    // Trim Windows CR if present
    if (len > 0 && buf[len - 1] == '\r') {
        len -= 1;
    }

    buf[len] = '\0';
    return buf;
}

int program_exit_code = 0;
// Per-call return propagation state
static int return_flag = 0;
static int break_flag = 0;
static int continue_flag = 0;
static Value return_value_holder;


Value interpret_ast(ASTNode* node, Environment* env) {
    if (node == NULL) {
        // Return a default Value for null nodes, e.g., an integer 0
        return create_value_int(0);
    }

#ifdef TON_DEBUG
    fprintf(stderr, "DEBUG: interpret_ast node type %d\n", node->type);
#endif

    switch (node->type) {
        case NODE_PROGRAM: {
            ProgramNode* program_node = (ProgramNode*)node;
#ifdef TON_DEBUG
            fprintf(stderr, "DEBUG: Interpreting program with %d statements\n", program_node->num_statements);
#endif
            // Interpret each statement in the program
            for (int i = 0; i < program_node->num_statements; i++) {
                interpret_ast(program_node->statements[i], env);
            }
            return create_value_int(0); // Program execution typically returns 0 on success
        }
        case NODE_VAR_DECLARATION: {
            VariableDeclarationNode* var_decl = (VariableDeclarationNode*)node;
            
            if (var_decl->var_type == VAR_TYPE_POINTER) {
                // Handle pointer variable declaration
                void* ptr_result = NULL;
                if (var_decl->initializer) {
                    Value init_val = interpret_ast(var_decl->initializer, env);
                    if (init_val.type == VALUE_POINTER) {
                        ptr_result = init_val.data.pointer_val;
                    } else if (init_val.type == VALUE_INT && init_val.data.int_val == 0) {
                        ptr_result = NULL; // Allow null pointer initialization
                    } else {
                        fprintf(stderr, "Runtime Error: Pointer variable initializer must be a pointer or null.\n");
                        exit(1);
                    }
                }
                env_add_variable(env, var_decl->identifier, create_value_pointer(ptr_result));
                return create_value_int(0);
            } else if (var_decl->var_type == VAR_TYPE_STRING) {
                // Handle string variable declaration
                char* string_result = NULL;
                if (var_decl->initializer) {
                    Value init_val = interpret_ast(var_decl->initializer, env);
                    if (init_val.type != VALUE_STRING) {
                        fprintf(stderr, "Runtime Error: String variable initializer must be a string.\n");
                        exit(1);
                    }
                    string_result = strdup(init_val.data.string_val);
                }
                env_add_variable(env, var_decl->identifier, create_value_string(string_result));
                return create_value_int(0);
            } else if (var_decl->var_type == VAR_TYPE_ARRAY) {
                // Handle array variable declaration
                TonArray* array_result = NULL;
                if (var_decl->initializer) {
                    Value init_val = interpret_ast(var_decl->initializer, env);
                    if (init_val.type != VALUE_ARRAY) {
                        fprintf(stderr, "Runtime Error: Array variable initializer must be an array.\n");
                        exit(1);
                    }
                    array_result = (TonArray*)init_val.data.array_val;
                }
                env_add_variable(env, var_decl->identifier, create_value_array(array_result));
                return create_value_int(0);
            } else if (var_decl->var_type == VAR_TYPE_BOOL) {
                // Handle boolean variable declaration
                int bool_result = 0;
                if (var_decl->initializer) {
                    Value init_val = interpret_ast(var_decl->initializer, env);
                    if (init_val.type != VALUE_INT && init_val.type != VALUE_BOOL) {
                        fprintf(stderr, "Runtime Error: Boolean variable initializer must be a boolean.\n");
                        exit(1);
                    }
                    bool_result = (init_val.type == VALUE_BOOL) ? init_val.data.bool_val : init_val.data.int_val;
                }
                env_add_variable(env, var_decl->identifier, create_value_bool(bool_result));
                return create_value_int(0);
            } else if (var_decl->var_type == VAR_TYPE_FLOAT) {
                // Handle float variable declaration
                double float_result = 0.0;
                if (var_decl->initializer) {
                    Value init_val = interpret_ast(var_decl->initializer, env);
                    if (init_val.type != VALUE_FLOAT && init_val.type != VALUE_INT) {
                        fprintf(stderr, "Runtime Error: Float variable initializer must be a float or int.\n");
                        exit(1);
                    }
                    float_result = (init_val.type == VALUE_FLOAT) ? init_val.data.float_val : (double)init_val.data.int_val;
                }
                env_add_variable(env, var_decl->identifier, create_value_float(float_result));
                return create_value_int(0);
            } else {
                // Handle integer and other variable declarations
                int int_result = 0;
                if (var_decl->initializer) {
                    Value init_val = interpret_ast(var_decl->initializer, env);
                    if (init_val.type != VALUE_INT) {
                        fprintf(stderr, "Runtime Error: Variable initializer must be an integer for now.\n");
                        exit(1);
                    }
                    int_result = init_val.data.int_val;
                }
                env_add_variable(env, var_decl->identifier, create_value_int(int_result));
                return create_value_int(0);
            }
        }
        case NODE_FN_DECLARATION: {
            FunctionDeclarationNode* fn_decl = (FunctionDeclarationNode*)node;
            Function* fn = (Function*)malloc(sizeof(Function));
            if (fn == NULL) {
                perror("Failed to allocate Function");
                exit(1);
            }
            fn->name = strdup(fn_decl->identifier->lexeme);
            fn->body = (ASTNode*)fn_decl->body;
            fn->closure_env = env; // Capture the environment where the function is declared
            // Capture parameters and return type for argument mapping
            fn->parameters = fn_decl->parameters;
            fn->num_parameters = fn_decl->num_parameters;
            fn->return_type = fn_decl->return_type;
            env_add_function(env, fn->name, fn);
#ifdef TON_DEBUG
            fprintf(stderr, "DEBUG: Registered function '%s'\n", fn->name);
#endif
            return create_value_int(0); // Function declarations don't return a value directly
        }
        case NODE_STRUCT_DECLARATION: {
            struct StructDeclarationNode* struct_decl = (struct StructDeclarationNode*)node;
            
            // Create struct fields array
            StructField* fields = malloc(struct_decl->num_fields * sizeof(StructField));
            for (int i = 0; i < struct_decl->num_fields; i++) {
                fields[i].name = strdup(struct_decl->field_names[i]);
                // Convert VariableType to string
                switch (struct_decl->field_types[i]) {
                    case VAR_TYPE_INT: fields[i].type_name = "int"; break;
                    case VAR_TYPE_FLOAT: fields[i].type_name = "float"; break;
                    case VAR_TYPE_STRING: fields[i].type_name = "string"; break;
                    case VAR_TYPE_BOOL: fields[i].type_name = "bool"; break;
                    default: fields[i].type_name = "unknown"; break;
                }
            }
            
            // Create methods array
            StructMethod* methods = NULL;
            if (struct_decl->num_methods > 0) {
                methods = malloc(struct_decl->num_methods * sizeof(StructMethod));
                for (int i = 0; i < struct_decl->num_methods; i++) {
                    FunctionDeclarationNode* method = struct_decl->methods[i];
                    methods[i].name = method->identifier->lexeme;
                    methods[i].function = method;
                }
            }
            
            // Define the struct type with methods
            TonStructType* struct_type = define_struct_type(
                struct_decl->name, 
                fields, 
                struct_decl->num_fields,
                methods,
                struct_decl->num_methods
            );
            
            // Store struct type in environment (for future use)
            // For now, we'll just print that it was defined
#ifdef TON_DEBUG
            fprintf(stderr, "DEBUG: Defined struct '%s' with %d fields and %d methods\n", 
                    struct_decl->name, struct_decl->num_fields, struct_decl->num_methods);
#endif
            
            return create_value_int(0);
        }
        case NODE_BLOCK_STATEMENT: {
            BlockStatementNode* block_node = (BlockStatementNode*)node;
            Value last_value = create_value_int(0);
            for (int i = 0; i < block_node->num_statements; i++) {
                last_value = interpret_ast(block_node->statements[i], env);
                if (return_flag) {
                    // short-circuit on return from inside the block
                    return return_value_holder;
                }
            }
            return last_value;
        }
        case NODE_IF_STATEMENT: {
            IfStatementNode* if_stmt = (IfStatementNode*)node;
            Value cond = interpret_ast(if_stmt->condition, env);
            if (cond.type != VALUE_INT && cond.type != VALUE_BOOL) {
                fprintf(stderr, "Runtime Error: if condition must evaluate to an integer or boolean.\n");
                exit(1);
            }
            int condition_result = 0;
            if (cond.type == VALUE_BOOL) {
                condition_result = cond.data.bool_val;
            } else {
                condition_result = cond.data.int_val != 0;  // non-zero means true
            }
            if (condition_result) {
                return interpret_ast((ASTNode*)if_stmt->consequence, env);
            } else if (if_stmt->alternative) {
                return interpret_ast((ASTNode*)if_stmt->alternative, env);
            } else {
                return create_value_int(0);
            }
        }
        case NODE_WHILE_STATEMENT: {
            WhileStatementNode* wh = (WhileStatementNode*)node;
            for (;;) {
                Value cond = interpret_ast(wh->condition, env);
                int condition_result = 0;
                if (cond.type == VALUE_INT) {
                    condition_result = cond.data.int_val != 0;  // non-zero means true
                } else if (cond.type == VALUE_BOOL) {
                    condition_result = cond.data.bool_val;
                } else {
                    fprintf(stderr, "Runtime Error: while condition must evaluate to an integer or boolean.\n");
                    exit(1);
                }
                if (!condition_result) {
                    return create_value_int(0);
                }
                break_flag = 0;
                continue_flag = 0;
                Value v = interpret_ast((ASTNode*)wh->body, env);
                if (return_flag) return v;
                if (break_flag) { break_flag = 0; return create_value_int(0); }
                if (continue_flag) { continue_flag = 0; continue; }
            }
        }
        case NODE_FOR_STATEMENT: {
            ForStatementNode* for_stmt = (ForStatementNode*)node;
            if (for_stmt->init) {
                interpret_ast(for_stmt->init, env);
            }
            while (1) {
                if (for_stmt->condition) {
                    Value cond = interpret_ast(for_stmt->condition, env);
                    if (cond.type != VALUE_INT) {
                        fprintf(stderr, "Runtime Error: for condition must evaluate to an integer (0/1).\n");
                        exit(1);
                    }
                    if (cond.data.int_val == 0) {
                        break; // Exit loop
                    }
                }
                break_flag = 0;
                continue_flag = 0;
                Value v = interpret_ast((ASTNode*)for_stmt->body, env);
                if (return_flag) return v;
                if (break_flag) { break_flag = 0; break; }
                if (continue_flag) { continue_flag = 0; }
                if (for_stmt->update) {
                    interpret_ast(for_stmt->update, env);
                }
            }
            return create_value_int(0);
        }
        case NODE_LOOP_STATEMENT: {
            LoopStatementNode* loop_stmt = (LoopStatementNode*)node;
            while (1) {
                break_flag = 0;
                continue_flag = 0;
                Value v = interpret_ast((ASTNode*)loop_stmt->body, env);
                if (return_flag) return v;
                if (break_flag) { break_flag = 0; return create_value_int(0); }
                if (continue_flag) { continue_flag = 0; continue; }
            }
        }
        case NODE_SWITCH_STATEMENT: {
            SwitchStatementNode* switch_stmt = (SwitchStatementNode*)node;
            Value switch_value = interpret_ast(switch_stmt->expression, env);
            
            if (switch_value.type != VALUE_INT) {
                fprintf(stderr, "Runtime Error: switch expression must evaluate to an integer.\n");
                exit(1);
            }
            
            // Check each case
            for (int i = 0; i < switch_stmt->num_cases; i++) {
                CaseStatementNode* case_stmt = switch_stmt->cases[i];
                Value case_value = interpret_ast(case_stmt->value, env);
                
                if (case_value.type != VALUE_INT) {
                    fprintf(stderr, "Runtime Error: case value must be an integer.\n");
                    exit(1);
                }
                
                if (switch_value.data.int_val == case_value.data.int_val) {
                    // Execute all statements in this case
                    Value last_value = create_value_int(0);
                    for (int j = 0; j < case_stmt->num_statements; j++) {
                        last_value = interpret_ast(case_stmt->statements[j], env);
                        if (return_flag) return last_value;
                    }
                    return last_value;
                }
            }
            
            // If no case matched, execute default case if it exists
            if (switch_stmt->default_case) {
                return interpret_ast((ASTNode*)switch_stmt->default_case, env);
            }
            
            return create_value_int(0);
        }
        case NODE_BREAK_STATEMENT: {
            break_flag = 1;
            return create_value_int(0);
        }
        case NODE_CONTINUE_STATEMENT: {
            continue_flag = 1;
            return create_value_int(0);
        }
        case NODE_EXPRESSION_STATEMENT: {
            ExpressionStatementNode* expr_stmt = (ExpressionStatementNode*)node;
            Value tmp = interpret_ast(expr_stmt->expression, env);
            // If expression yields a temporary string, free it here
            if (tmp.type == VALUE_STRING) {
                free(tmp.data.string_val);
            }
            // Expression statements do not yield a meaningful value
            return create_value_int(0);
        }
        case NODE_RETURN_STATEMENT: {
            ReturnStatementNode* ret_stmt = (ReturnStatementNode*)node;
            Value return_value = interpret_ast(ret_stmt->expression, env);
            program_exit_code = (return_value.type == VALUE_INT) ? return_value.data.int_val : program_exit_code; // Set exit code if int
            // mark return for the current call context
            return_flag = 1;
            return_value_holder = return_value;
            return return_value;
        }
        case NODE_PRINT_STATEMENT: {
            struct PrintStatementNode* print_stmt = (struct PrintStatementNode*)node;
            Value val = interpret_ast(print_stmt->expression, env);
            if (val.type == VALUE_INT) {
                printf("%d\n", val.data.int_val);
            } else if (val.type == VALUE_STRING) {
                printf("%s\n", val.data.string_val);
                // Free temporary string values produced by expressions
                free(val.data.string_val);
            } else if (val.type == VALUE_FLOAT) {
                printf("%.6f\n", val.data.float_val);
            } else if (val.type == VALUE_BOOL) {
                printf("%s\n", val.data.bool_val ? "true" : "false");
            } else if (val.type == VALUE_NULL) {
                printf("null\n");
            } else if (val.type == VALUE_POINTER) {
                printf("pointer@%p\n", val.data.pointer_val);
            } else if (val.type == VALUE_TONLIST) {
                printf("TonList@%p\n", val.data.tonlist_val);
            } else if (val.type == VALUE_TONMAP) {
                printf("TonMap@%p\n", val.data.tonmap_val);
            } else if (val.type == VALUE_TONSET) {
                printf("TonSet@%p\n", val.data.tonset_val);
            } else {
                fprintf(stderr, "Runtime Error: Unsupported value type for printing.\n");
                exit(1);
            }
            return create_value_int(0); // Print statement doesn't return a value
        }
        case NODE_LITERAL_EXPRESSION:
            {
                LiteralExpressionNode* lit_expr = (LiteralExpressionNode*)node;
                if (lit_expr->value->type == TOKEN_INT_LITERAL) {
                    return create_value_int(atoi(lit_expr->value->lexeme));
                } else if (lit_expr->value->type == TOKEN_STRING_LITERAL) {
                    // Remove quotes from the string literal
                    char* str_val = strdup(lit_expr->value->lexeme);
                    str_val[strlen(str_val) - 1] = '\0'; // Remove closing quote
                    Value result = create_value_string(str_val + 1); // Skip opening quote
                    free(str_val); // Free the duplicated string
                    return result;
                } else if (lit_expr->value->type == TOKEN_TRUE || lit_expr->value->type == TOKEN_FALSE) {
                    return create_value_int(lit_expr->value->type == TOKEN_TRUE ? 1 : 0);
                } else if (lit_expr->value->type == TOKEN_FLOAT_LITERAL) {
                    return create_value_float(atof(lit_expr->value->lexeme));
                } else {
                    fprintf(stderr, "Runtime Error: Only integer, boolean, float and string literals are supported for now.\n");
                    exit(1);
                }
            }
        case NODE_IDENTIFIER_EXPRESSION: {
            IdentifierExpressionNode* id_expr = (IdentifierExpressionNode*)node;
            // Get the Value directly from environment
            Value value = env_get_variable(env, id_expr->identifier);
            return value;
        }
        case NODE_FN_CALL_EXPRESSION: {
            FunctionCallExpressionNode* call_expr = (FunctionCallExpressionNode*)node;
            // The callee should be an identifier referring to a function
            if (call_expr->callee->type != NODE_IDENTIFIER_EXPRESSION) {
                fprintf(stderr, "Runtime Error: Callee in a call expression must be an identifier.\n");
                exit(1);
            }
            IdentifierExpressionNode* callee_id = (IdentifierExpressionNode*)call_expr->callee;
            char* function_name = callee_id->identifier;
            int argc = call_expr->num_arguments;

            // Evaluate arguments - check if it's a TonLib function first
            Value tonlib_result = call_tonlib_function(function_name, call_expr->arguments, argc, env);
            if (tonlib_result.type != VALUE_NULL) {
                return tonlib_result;
            }

            // For built-in bitwise functions and user functions, still require integer arguments
            int arg_ints[8];
            if (argc > 8) {
                fprintf(stderr, "Runtime Error: Too many arguments (%d).\n", argc);
                exit(1);
            }
            for (int i = 0; i < argc; ++i) {
                Value arg_val = interpret_ast(call_expr->arguments[i], env);
                if (arg_val.type != VALUE_INT) {
                    fprintf(stderr, "Runtime Error: Only integer arguments are supported for built-in bitwise and user functions for now.\n");
                    exit(1);
                }
                arg_ints[i] = arg_val.data.int_val;
            }

            if (strcmp(function_name, "read_line") == 0) {
                // Handle read_line built-in function (no arguments)
                if (argc > 0) {
                    fprintf(stderr, "Runtime Error: read_line() does not take any arguments.\n");
                    exit(1);
                }

                char* line = read_line_alloc();
                if (line == NULL) {
                    // Allocation failed; treat as fatal for now
                    fprintf(stderr, "Runtime Error: Failed to allocate memory for input line.\n");
                    exit(1);
                }
                // Convert to Value and free temporary buffer
                Value str_val = create_value_string(line);
                free(line);
                return str_val;
            } else if (strcmp(function_name, "bit_and") == 0) {
                if (argc != 2) { fprintf(stderr, "Runtime Error: bit_and(a, b) requires 2 arguments.\n"); exit(1);} 
                return create_value_int(ton_bit_and(arg_ints[0], arg_ints[1]));
            } else if (strcmp(function_name, "bit_or") == 0) {
                if (argc != 2) { fprintf(stderr, "Runtime Error: bit_or(a, b) requires 2 arguments.\n"); exit(1);} 
                return create_value_int(ton_bit_or(arg_ints[0], arg_ints[1]));
            } else if (strcmp(function_name, "bit_xor") == 0) {
                if (argc != 2) { fprintf(stderr, "Runtime Error: bit_xor(a, b) requires 2 arguments.\n"); exit(1);} 
                return create_value_int(ton_bit_xor(arg_ints[0], arg_ints[1]));
            } else if (strcmp(function_name, "bit_not") == 0) {
                if (argc != 1) { fprintf(stderr, "Runtime Error: bit_not(a) requires 1 argument.\n"); exit(1);} 
                return create_value_int(ton_bit_not(arg_ints[0]));
            } else if (strcmp(function_name, "bit_shl") == 0) {
                if (argc != 2) { fprintf(stderr, "Runtime Error: bit_shl(a, n) requires 2 arguments.\n"); exit(1);} 
                return create_value_int(ton_shift_left(arg_ints[0], arg_ints[1]));
            } else if (strcmp(function_name, "bit_shr") == 0) {
                if (argc != 2) { fprintf(stderr, "Runtime Error: bit_shr(a, n) requires 2 arguments.\n"); exit(1);} 
                return create_value_int(ton_shift_right(arg_ints[0], arg_ints[1]));
            } else {
                // Handle user-defined functions
                Function* func = env_get_function(env, function_name);

                if (func == NULL) {
                    fprintf(stderr, "Runtime Error: Call to undefined function '%s'.\n", function_name);
                    exit(1);
                }

                // Create a new environment for the function call, inheriting from the function's closure environment
                Environment* func_env = create_child_environment(func->closure_env);

                // Map arguments to parameters (currently only int type is supported)
                if (argc != func->num_parameters) {
                    fprintf(stderr, "Runtime Error: Function '%s' expects %d arguments, got %d.\n", function_name, func->num_parameters, argc);
                    destroy_environment(func_env);
                    exit(1);
                }
                for (int i = 0; i < argc; ++i) {
                    ParameterNode* param = func->parameters[i];
                    if (param->param_type != VAR_TYPE_INT) {
                        fprintf(stderr, "Runtime Error: Only int parameters are supported for now in user-defined functions.\n");
                        destroy_environment(func_env);
                        exit(1);
                    }
                    env_add_variable(func_env, param->identifier->lexeme, create_value_int(arg_ints[i]));
                }

                // Save and reset return state for this call
                int saved_return_flag = return_flag;
                Value saved_return_value = return_value_holder;
                return_flag = 0;

                Value result = interpret_ast(func->body, func_env);
                if (return_flag) {
                    result = return_value_holder;
                }

                // Restore caller's return state
                return_flag = saved_return_flag;
                return_value_holder = saved_return_value;

                destroy_environment(func_env); // Clean up the function call environment
                return result;
            }
        }
        case NODE_BINARY_EXPRESSION: {
            BinaryExpressionNode* bin = (BinaryExpressionNode*)node;
            
            // Handle assignment operators specially
            if (bin->operator->type == TOKEN_ASSIGN || 
                bin->operator->type == TOKEN_PLUS_ASSIGN ||
                bin->operator->type == TOKEN_MINUS_ASSIGN ||
                bin->operator->type == TOKEN_STAR_ASSIGN ||
                bin->operator->type == TOKEN_SLASH_ASSIGN ||
                bin->operator->type == TOKEN_MODULO_ASSIGN) {
                
                // Left side must be an identifier
                if (bin->left->type != NODE_IDENTIFIER_EXPRESSION) {
                    fprintf(stderr, "Runtime Error: Left side of assignment must be a variable.\n");
                    exit(1);
                }
                IdentifierExpressionNode* id_expr = (IdentifierExpressionNode*)bin->left;
                
                if (bin->operator->type == TOKEN_ASSIGN) {
                    Value right = interpret_ast(bin->right, env);
                    env_set_variable(env, id_expr->identifier, right);
                    return right; // Assignment returns the assigned value
                } else {
                    // Compound assignment operators
                    Value current = env_get_variable(env, id_expr->identifier);
                    Value right = interpret_ast(bin->right, env);
                    Value result;
                    
                    // Perform the operation based on the operator
                    switch (bin->operator->type) {
                        case TOKEN_PLUS_ASSIGN:
                            if (current.type == VALUE_INT && right.type == VALUE_INT) {
                                result = create_value_int(current.data.int_val + right.data.int_val);
                            } else if (current.type == VALUE_FLOAT && right.type == VALUE_FLOAT) {
                                result = create_value_float(current.data.float_val + right.data.float_val);
                            } else {
                                fprintf(stderr, "Runtime Error: Type mismatch in += operation.\n");
                                exit(1);
                            }
                            break;
                        case TOKEN_MINUS_ASSIGN:
                            if (current.type == VALUE_INT && right.type == VALUE_INT) {
                                result = create_value_int(current.data.int_val - right.data.int_val);
                            } else if (current.type == VALUE_FLOAT && right.type == VALUE_FLOAT) {
                                result = create_value_float(current.data.float_val - right.data.float_val);
                            } else {
                                fprintf(stderr, "Runtime Error: Type mismatch in -= operation.\n");
                                exit(1);
                            }
                            break;
                        case TOKEN_STAR_ASSIGN:
                            if (current.type == VALUE_INT && right.type == VALUE_INT) {
                                result = create_value_int(current.data.int_val * right.data.int_val);
                            } else if (current.type == VALUE_FLOAT && right.type == VALUE_FLOAT) {
                                result = create_value_float(current.data.float_val * right.data.float_val);
                            } else {
                                fprintf(stderr, "Runtime Error: Type mismatch in *= operation.\n");
                                exit(1);
                            }
                            break;
                        case TOKEN_SLASH_ASSIGN:
                            if (current.type == VALUE_INT && right.type == VALUE_INT) {
                                if (right.data.int_val == 0) {
                                    fprintf(stderr, "Runtime Error: Division by zero in /= operation.\n");
                                    exit(1);
                                }
                                result = create_value_int(current.data.int_val / right.data.int_val);
                            } else if (current.type == VALUE_FLOAT && right.type == VALUE_FLOAT) {
                                if (right.data.float_val == 0.0) {
                                    fprintf(stderr, "Runtime Error: Division by zero in /= operation.\n");
                                    exit(1);
                                }
                                result = create_value_float(current.data.float_val / right.data.float_val);
                            } else {
                                fprintf(stderr, "Runtime Error: Type mismatch in /= operation.\n");
                                exit(1);
                            }
                            break;
                        case TOKEN_MODULO_ASSIGN:
                            if (current.type == VALUE_INT && right.type == VALUE_INT) {
                                if (right.data.int_val == 0) {
                                    fprintf(stderr, "Runtime Error: Modulo by zero in %%= operation.\n");
                                    exit(1);
                                }
                                result = create_value_int(current.data.int_val % right.data.int_val);
                            } else {
                                fprintf(stderr, "Runtime Error: Modulo operation only supported for integers.\n");
                                exit(1);
                            }
                            break;
                        default:
                            fprintf(stderr, "Runtime Error: Unknown assignment operator.\n");
                            exit(1);
                    }
                    
                    env_set_variable(env, id_expr->identifier, result);
                    return result;
                }
            }
            
            Value left = interpret_ast(bin->left, env);
            Value right = interpret_ast(bin->right, env);
            
            // Handle pointer comparisons
            if (left.type == VALUE_POINTER && right.type == VALUE_POINTER) {
                switch (bin->operator->type) {
                    case TOKEN_EQ:
                        return create_value_int(left.data.pointer_val == right.data.pointer_val);
                    case TOKEN_NEQ:
                        return create_value_int(left.data.pointer_val != right.data.pointer_val);
                    default:
                        fprintf(stderr, "Runtime Error: Unsupported operation for pointers.\n");
                        exit(1);
                }
            }
            
            // Handle pointer vs integer comparisons (for NULL checks)
            if ((left.type == VALUE_POINTER && right.type == VALUE_INT) ||
                (left.type == VALUE_INT && right.type == VALUE_POINTER)) {
                void* ptr_val = (left.type == VALUE_POINTER) ? left.data.pointer_val : right.data.pointer_val;
                int int_val = (left.type == VALUE_INT) ? left.data.int_val : right.data.int_val;
                
                switch (bin->operator->type) {
                    case TOKEN_EQ:
                        // NULL pointer (0x0) equals 0, non-NULL pointer does not equal 0
                        if (int_val == 0) {
                            return create_value_int(ptr_val == NULL ? 1 : 0);
                        } else {
                            return create_value_int(0); // Non-zero integer never equals any pointer
                        }
                    case TOKEN_NEQ:
                        // Opposite of equality
                        if (int_val == 0) {
                            return create_value_int(ptr_val == NULL ? 0 : 1);
                        } else {
                            return create_value_int(1); // Non-zero integer never equals any pointer
                        }
                    default:
                        fprintf(stderr, "Runtime Error: Unsupported operation for pointer-integer comparison.\n");
                        exit(1);
                }
            }
            
            // Handle string concatenation with + operator
            if (left.type == VALUE_STRING && right.type == VALUE_STRING && bin->operator->type == TOKEN_PLUS) {
                size_t left_len = strlen(left.data.string_val);
                size_t right_len = strlen(right.data.string_val);
                char* result = malloc(left_len + right_len + 1);
                if (result == NULL) {
                    fprintf(stderr, "Runtime Error: Memory allocation failed for string concatenation.\n");
                    exit(1);
                }
                strcpy(result, left.data.string_val);
                strcat(result, right.data.string_val);
                return create_value_string(result);
            }
            
            // Handle string concatenation with integer/array values
            if (bin->operator->type == TOKEN_PLUS && (left.type == VALUE_STRING || right.type == VALUE_STRING)) {
                char* left_str = NULL;
                char* right_str = NULL;
                int free_left = 0, free_right = 0;
                
                // Convert left operand to string
                if (left.type == VALUE_STRING) {
                    left_str = left.data.string_val;
                } else if (left.type == VALUE_INT) {
                    left_str = malloc(32);
                    sprintf(left_str, "%d", left.data.int_val);
                    free_left = 1;
                } else if (left.type == VALUE_ARRAY) {
                    left_str = strdup("[array]");
                    free_left = 1;
                } else {
                    left_str = strdup("[unknown]");
                    free_left = 1;
                }
                
                // Convert right operand to string
                if (right.type == VALUE_STRING) {
                    right_str = right.data.string_val;
                } else if (right.type == VALUE_INT) {
                    right_str = malloc(32);
                    sprintf(right_str, "%d", right.data.int_val);
                    free_right = 1;
                } else if (right.type == VALUE_ARRAY) {
                    right_str = strdup("[array]");
                    free_right = 1;
                } else {
                    right_str = strdup("[unknown]");
                    free_right = 1;
                }
                
                // Concatenate strings
                size_t left_len = strlen(left_str);
                size_t right_len = strlen(right_str);
                char* result = malloc(left_len + right_len + 1);
                if (result == NULL) {
                    fprintf(stderr, "Runtime Error: Memory allocation failed for string concatenation.\n");
                    exit(1);
                }
                strcpy(result, left_str);
                strcat(result, right_str);
                
                // Free temporary strings
                if (free_left) free(left_str);
                if (free_right) free(right_str);
                
                return create_value_string(result);
            }
            
            // Handle string comparisons
            if (left.type == VALUE_STRING && right.type == VALUE_STRING) {
                switch (bin->operator->type) {
                    case TOKEN_EQ:
                        return create_value_int(strcmp(left.data.string_val, right.data.string_val) == 0);
                    case TOKEN_NEQ:
                        return create_value_int(strcmp(left.data.string_val, right.data.string_val) != 0);
                    default:
                        fprintf(stderr, "Runtime Error: Unsupported string comparison operator.\n");
                        exit(1);
                }
            }
            
            if (left.type != VALUE_INT || right.type != VALUE_INT) {
                fprintf(stderr, "Runtime Error: Binary operations support only integers for now.\n");
                exit(1);
            }
            switch (bin->operator->type) {
                case TOKEN_PLUS:
                    return create_value_int(left.data.int_val + right.data.int_val);
                case TOKEN_MINUS:
                    return create_value_int(left.data.int_val - right.data.int_val);
                case TOKEN_STAR:
                    return create_value_int(left.data.int_val * right.data.int_val);
                case TOKEN_SLASH:
                    if (right.data.int_val == 0) { fprintf(stderr, "Runtime Error: Division by zero.\n"); exit(1);} 
                    return create_value_int(left.data.int_val / right.data.int_val);
                case TOKEN_EQ:
                    return create_value_int(left.data.int_val == right.data.int_val);
                case TOKEN_NEQ:
                    return create_value_int(left.data.int_val != right.data.int_val);
                case TOKEN_LT:
                    return create_value_int(left.data.int_val < right.data.int_val);
                case TOKEN_LE:
                    return create_value_int(left.data.int_val <= right.data.int_val);
                case TOKEN_GT:
                    return create_value_int(left.data.int_val > right.data.int_val);
                case TOKEN_GE:
                    return create_value_int(left.data.int_val >= right.data.int_val);
                default:
                    fprintf(stderr, "Runtime Error: Unsupported binary operator.\n");
                    exit(1);
            }
        }
        case NODE_UNARY_EXPRESSION: {
            UnaryExpressionNode* un = (UnaryExpressionNode*)node;
            
            switch (un->operator->type) {
                case TOKEN_STAR: { // Dereference operator
                    Value operand = interpret_ast(un->operand, env);
                    if (operand.type != VALUE_POINTER) {
                        fprintf(stderr, "Runtime Error: Cannot dereference non-pointer value.\n");
                        exit(1);
                    }
                    // For now, return a simple integer from the pointer
                    // In a full implementation, we'd need to know the type of data pointed to
                    int* ptr = (int*)operand.data.pointer_val;
                    if (!ptr) {
                        fprintf(stderr, "Runtime Error: Cannot dereference null pointer.\n");
                        exit(1);
                    }
                    return create_value_int(*ptr);
                }
                case TOKEN_AMPERSAND: { // Address operator
                    // For now, we can only take address of variables
                    if (un->operand->type != NODE_IDENTIFIER_EXPRESSION) {
                        fprintf(stderr, "Runtime Error: Can only take address of variables for now.\n");
                        exit(1);
                    }
                    IdentifierExpressionNode* id_expr = (IdentifierExpressionNode*)un->operand;
                    Value var_value = env_get_variable(env, id_expr->identifier);
                    
                    // Create a pointer to the variable's data
                    // Note: This is a simplified implementation
                    void* addr = NULL;
                    switch (var_value.type) {
                        case VALUE_INT:
                            addr = &var_value.data.int_val;
                            break;
                        case VALUE_FLOAT:
                            addr = &var_value.data.float_val;
                            break;
                        default:
                            fprintf(stderr, "Runtime Error: Cannot take address of this type.\n");
                            exit(1);
                    }
                    return create_value_pointer(addr);
                }
                default: {
                    // For increment/decrement operators, we need to handle them specially
                    if (un->operator->type == TOKEN_INCREMENT || un->operator->type == TOKEN_DECREMENT) {
                        // These operators require the operand to be a variable (identifier)
                        if (un->operand->type != NODE_IDENTIFIER_EXPRESSION) {
                            fprintf(stderr, "Runtime Error: Increment/decrement operators can only be applied to variables.\n");
                            exit(1);
                        }
                        
                        IdentifierExpressionNode* id_expr = (IdentifierExpressionNode*)un->operand;
                        Value current_value = env_get_variable(env, id_expr->identifier);
                        
                        if (current_value.type != VALUE_INT && current_value.type != VALUE_FLOAT) {
                            fprintf(stderr, "Runtime Error: Increment/decrement operators support only integers and floats.\n");
                            exit(1);
                        }
                        
                        Value new_value;
                        if (current_value.type == VALUE_INT) {
                            if (un->operator->type == TOKEN_INCREMENT) {
                                new_value = create_value_int(current_value.data.int_val + 1);
                            } else {
                                new_value = create_value_int(current_value.data.int_val - 1);
                            }
                        } else { // VALUE_FLOAT
                            if (un->operator->type == TOKEN_INCREMENT) {
                                new_value = create_value_float(current_value.data.float_val + 1.0);
                            } else {
                                new_value = create_value_float(current_value.data.float_val - 1.0);
                            }
                        }
                        
                        // Update the variable in the environment
                        env_set_variable(env, id_expr->identifier, new_value);
                        
                        // Return the new value (for both prefix and postfix, we'll return the new value for simplicity)
                        return new_value;
                    }
                    
                    Value operand = interpret_ast(un->operand, env);
                    if (operand.type != VALUE_INT) {
                        fprintf(stderr, "Runtime Error: Unary operators support only integers for now.\n");
                        exit(1);
                    }
                    switch (un->operator->type) {
                        case TOKEN_MINUS:
                            return create_value_int(-operand.data.int_val);
                        case TOKEN_NOT:
                            return create_value_int(operand.data.int_val == 0 ? 1 : 0);
                        default:
                            fprintf(stderr, "Runtime Error: Unsupported unary operator.\n");
                            exit(1);
                    }
                }
            }
        }
        case NODE_CONDITIONAL_EXPRESSION: {
            ConditionalExpressionNode* cond = (ConditionalExpressionNode*)node;
            Value condition = interpret_ast(cond->condition, env);
            
            // Convert condition to boolean
            int is_true = 0;
            switch (condition.type) {
                case VALUE_INT:
                    is_true = (condition.data.int_val != 0);
                    break;
                case VALUE_FLOAT:
                    is_true = (condition.data.float_val != 0.0);
                    break;
                case VALUE_BOOL:
                    is_true = condition.data.bool_val;
                    break;
                case VALUE_STRING:
                    is_true = (condition.data.string_val != NULL && strlen(condition.data.string_val) > 0);
                    break;
                case VALUE_POINTER:
                    is_true = (condition.data.pointer_val != NULL);
                    break;
                default:
                    is_true = 0;
                    break;
            }
            
            if (is_true) {
                return interpret_ast(cond->true_expr, env);
            } else {
                return interpret_ast(cond->false_expr, env);
            }
        }
        case NODE_TYPEOF_EXPRESSION: {
            TypeofExpressionNode* typeof_expr = (TypeofExpressionNode*)node;
            Value operand_value = interpret_ast(typeof_expr->operand, env);
            
            Value result;
            result.type = VALUE_STRING;
            
            switch (operand_value.type) {
                case VALUE_INT:
                    result.data.string_val = strdup("int");
                    break;
                case VALUE_FLOAT:
                    result.data.string_val = strdup("float");
                    break;
                case VALUE_BOOL:
                    result.data.string_val = strdup("bool");
                    break;
                case VALUE_STRING:
                    result.data.string_val = strdup("string");
                    break;
                case VALUE_POINTER:
                    result.data.string_val = strdup("pointer");
                    break;
                case VALUE_ARRAY:
                    result.data.string_val = strdup("array");
                    break;
                default:
                    result.data.string_val = strdup("unknown");
                    break;
            }
            
            return result;
        }
        case NODE_ARRAY_LITERAL_EXPRESSION: {
            ArrayLiteralExpressionNode* array_lit = (ArrayLiteralExpressionNode*)node;
            
            // Create a dynamic array and populate it with elements
            TonArray* arr = create_dynamic_array(array_lit->num_elements);
            if (!arr) {
                fprintf(stderr, "Runtime Error: Failed to create array.\n");
                exit(1);
            }
            
            for (size_t i = 0; i < array_lit->num_elements; i++) {
                Value element = interpret_ast(array_lit->elements[i], env);
                if (!array_push(arr, element)) {
                    fprintf(stderr, "Runtime Error: Failed to add element to array.\n");
                    destroy_array(arr);
                    exit(1);
                }
            }
            
            return create_value_array(arr);
        }
        case NODE_ARRAY_ACCESS_EXPRESSION: {
            ArrayAccessExpressionNode* array_access = (ArrayAccessExpressionNode*)node;
            
            // Get the array value
            Value array_val = interpret_ast(array_access->array, env);
            if (array_val.type != VALUE_ARRAY) {
                fprintf(stderr, "Runtime Error: Cannot index non-array value.\n");
                exit(1);
            }
            
            // Get the index value
            Value index_val = interpret_ast(array_access->index, env);
            if (index_val.type != VALUE_INT) {
                fprintf(stderr, "Runtime Error: Array index must be an integer.\n");
                exit(1);
            }
            
            TonArray* arr = (TonArray*)array_val.data.array_val;
            size_t index = (size_t)index_val.data.int_val;
            
            if (index >= arr->length) {
                fprintf(stderr, "Runtime Error: Array index out of bounds.\n");
                exit(1);
            }
            
            return array_get(arr, index);
        }
        case NODE_IMPORT_STATEMENT: {
            ImportStatementNode* import_stmt = (ImportStatementNode*)node;
            // TODO: Implement actual module loading
            // For now, just print that we're importing
            printf("Importing module: %s\n", import_stmt->module_path);
            return create_value_int(0);
        }
        // TODO: Implement other node types for interpretation
        default:
            fprintf(stderr, "Runtime Error: Unhandled AST node type %d\n", node->type);
            exit(1);
    }
}