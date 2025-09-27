#include <string.h>
#include "interpreter_expr.h"
#include "ast.h"
#include "environment.h"
#include "value.h"
#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "interpreter_stmt.h"
#include "array.h"
#include "collections.h"
#include "io.h"
#include "builtin_tonlib.h"
#include "builtin_crypto.h"
#include "interpreter_macro.h"



TonError interpret_expression(ASTNode* node, Environment* env, Value* out_result) {
    if (!node || !env || !out_result) {
        return ton_error(TON_ERR_RUNTIME, "Invalid arguments to interpret_expression", 0, 0, __FILE__);
    }
    *out_result = create_value_null(); // Default to null

    switch (node->type) {
        case NODE_LITERAL_EXPRESSION: {
            LiteralExpressionNode* lit_expr = (LiteralExpressionNode*)node;
            if (lit_expr->value->type == TOKEN_INT_LITERAL) {
                *out_result = create_value_int(atoi(lit_expr->value->lexeme));
            } else if (lit_expr->value->type == TOKEN_STRING_LITERAL) {
                char* temp_str = strdup(lit_expr->value->lexeme);
                temp_str[strlen(temp_str) - 1] = '\0';
                *out_result = create_value_string(temp_str + 1);
                free(temp_str);
            } else if (lit_expr->value->type == TOKEN_TRUE || lit_expr->value->type == TOKEN_FALSE) {
                *out_result = create_value_bool(lit_expr->value->type == TOKEN_TRUE);
            } else if (lit_expr->value->type == TOKEN_FLOAT_LITERAL) {
                *out_result = create_value_float(atof(lit_expr->value->lexeme));
            } else if (lit_expr->value->type == TOKEN_CHAR_LITERAL) {
                *out_result = create_value_char(lit_expr->value->lexeme[0]);
            } else if (lit_expr->value->type == TOKEN_NULL) {
                *out_result = create_value_null();
            } else {
                return ton_error(TON_ERR_RUNTIME, "Unsupported literal type", node->line, node->column, __FILE__);
            }
            return ton_ok();
        }
        case NODE_IDENTIFIER_EXPRESSION: {
            IdentifierExpressionNode* id_expr = (IdentifierExpressionNode*)node;

            // First try to find as a variable
            Value* var = env_get_variable(env, id_expr->identifier);
            if (var) {
                *out_result = *var;
                value_add_ref(out_result);
                return ton_ok();
            }
            
            // If not found as variable, try to find as a function
            Function* func = env_get_function(env, id_expr->identifier);
            if (func) {
                *out_result = create_value_fn(func);
                return ton_ok();
            }
            
            // Check for 'this' and field access only if we haven't found the identifier yet
            Value* this_val = env_get_variable(env, "this");
            if (this_val && this_val->type == VALUE_STRUCT) {
                TonStructInstance* instance = this_val->data.struct_val;
                Value field_val = struct_get_field(instance, id_expr->identifier);
                if (field_val.type != VALUE_NULL) {
                    *out_result = field_val;
                    value_add_ref(out_result);
                    return ton_ok();
                }
            }
            
            // Neither variable nor function found
            char error_msg[256];
            snprintf(error_msg, sizeof(error_msg), "Undefined variable '%s'", id_expr->identifier);
            return ton_error(TON_ERR_RUNTIME, error_msg, node->line, node->column, __FILE__);
        }
        case NODE_FN_CALL_EXPRESSION: {
            FunctionCallExpressionNode* call_node = (FunctionCallExpressionNode*)node;
            Value callee_val;
            TonError err = interpret_expression(call_node->callee, env, &callee_val);
            if (err.code != TON_OK) return err;

            // Check if it's a macro first
            if (callee_val.type == VALUE_MACRO) {
                value_release(&callee_val);
                
                // Convert function call to macro call
                MacroCallExpressionNode macro_call;
                macro_call.base.type = NODE_MACRO_CALL_EXPRESSION;
                macro_call.base.line = node->line;
                macro_call.base.column = node->column;
                
                // Get macro name from identifier
                if (call_node->callee->type == NODE_IDENTIFIER_EXPRESSION) {
                    IdentifierExpressionNode* id_node = (IdentifierExpressionNode*)call_node->callee;
                    macro_call.macro_name = id_node->identifier;
                } else {
                    return ton_error(TON_ERR_RUNTIME, "Invalid macro call", node->line, node->column, __FILE__);
                }
                
                macro_call.arguments = call_node->arguments;
                macro_call.num_arguments = call_node->num_arguments;
                
                *out_result = evaluate_macro_call_expression(&macro_call, env);
                return ton_ok();
            }

            if (callee_val.type != VALUE_FN) {
                value_release(&callee_val);
                return ton_error(TON_ERR_TYPE, "Cannot call non-function value", node->line, node->column, __FILE__);
            }

            Function* function = callee_val.data.function_value;
            value_release(&callee_val);

            // Handle built-in functions
            if (function->type == BUILT_IN) {
                Value* args = (Value*)ton_malloc(sizeof(Value) * call_node->num_arguments);
                if (!args) {
                    return ton_error(TON_ERR_MEMORY, "Memory allocation failed for arguments", node->line, node->column, __FILE__);
                }

                for (int i = 0; i < call_node->num_arguments; i++) {
                    err = interpret_expression(call_node->arguments[i], env, &args[i]);
                    if (err.code != TON_OK) {
                        for (int j = 0; j < i; j++) value_release(&args[j]);
                        ton_free(args);
                        return err;
                    }
                }

                // Dispatch to appropriate built-in function handler
                Value result;
                if (strcmp(function->name, "math_pi") == 0 ||
                    strcmp(function->name, "math_e") == 0 ||
                    strcmp(function->name, "list_create") == 0 ||
                    strcmp(function->name, "map_create") == 0 ||
                    strcmp(function->name, "set_create") == 0 ||
                    strcmp(function->name, "array_create") == 0 || // Dodano array_create
                    strcmp(function->name, "int_to_string") == 0 ||
                    strcmp(function->name, "float_to_string") == 0 ||
                    strcmp(function->name, "string_to_int") == 0 ||
                    strcmp(function->name, "string_to_float") == 0 ||
                    strcmp(function->name, "length") == 0 ||
                    strcmp(function->name, "concat") == 0 ||
                    strcmp(function->name, "substring") == 0 ||
                    strcmp(function->name, "string_to_int_base") == 0 ||
                    strcmp(function->name, "int_to_hex_upper") == 0 ||
                    strcmp(function->name, "upper_case") == 0 ||
                    strcmp(function->name, "lower_case") == 0 ||
                    strcmp(function->name, "strpos") == 0 ||
                    strcmp(function->name, "sha256") == 0) {
                    result = call_tonlib_function(function->name, args, call_node->num_arguments);
                } else if (strncmp(function->name, "sha256_hash", 11) == 0 ||
                          strncmp(function->name, "md5_hash", 8) == 0 ||
                          strncmp(function->name, "base64_encode_text", 18) == 0 ||
                          strncmp(function->name, "base64_decode_text", 18) == 0 ||
                          strcmp(function->name, "random_int") == 0 ||
                          strcmp(function->name, "random_string") == 0 ||
                          strcmp(function->name, "xor_strings") == 0 ||
                          strcmp(function->name, "caesar_encrypt") == 0 ||
                          strcmp(function->name, "caesar_decrypt") == 0 ||
                          strcmp(function->name, "rot13") == 0 ||
                          strcmp(function->name, "char_code") == 0 ||
                          strcmp(function->name, "char_from_code") == 0 ||
                          strcmp(function->name, "crypto_demo") == 0) {
                    result = call_crypto_function(function->name, args, call_node->num_arguments);
                } else {
                    // Handle other built-ins like read_line, bitwise operations
                    result = create_value_null();
                }

                for (int i = 0; i < call_node->num_arguments; i++) {
                    value_release(&args[i]);
                }
                ton_free(args);

                *out_result = result;
                return ton_ok();
            }

            // Handle user-defined functions
            if (call_node->num_arguments != function->num_parameters) {
                return ton_error(TON_ERR_TYPE, "Argument count mismatch", node->line, node->column, __FILE__);
            }

            Value* args = (Value*)ton_malloc(sizeof(Value) * call_node->num_arguments);
            if (!args) {
                return ton_error(TON_ERR_MEMORY, "Memory allocation failed for arguments", node->line, node->column, __FILE__);
            }

            for (int i = 0; i < call_node->num_arguments; i++) {
                err = interpret_expression(call_node->arguments[i], env, &args[i]);
                if (err.code != TON_OK) {
                    for (int j = 0; j < i; j++) value_release(&args[j]);
                    ton_free(args);
                    return err;
                }
            }

            Environment* fn_env = create_child_environment(function->closure_env ? function->closure_env : env);
 
             for (int i = 0; i < function->num_parameters; i++) {
                 ParameterNode* param = function->parameters[i];
                 value_add_ref(&args[i]);
                 env_add_variable(fn_env, param->identifier->lexeme, args[i], param->param_type);
             }
 
             err = interpret_statement(function->body, fn_env, out_result);
 
             for (int i = 0; i < call_node->num_arguments; i++) {
                 value_release(&args[i]);
             }
             ton_free(args);

             env_release(fn_env);
 
             if (err.code == TON_RETURN) {
                 value_add_ref(out_result);
                 return ton_ok();
             } else if (err.code != TON_OK) {
                 return err;
             }
            return ton_ok();
        }
        case NODE_BINARY_EXPRESSION: {
            BinaryExpressionNode* bin_node = (BinaryExpressionNode*)node;
            if (bin_node->operator->type == TOKEN_ASSIGN ||
                bin_node->operator->type == TOKEN_PLUS_ASSIGN ||
                bin_node->operator->type == TOKEN_MINUS_ASSIGN ||
                bin_node->operator->type == TOKEN_STAR_ASSIGN ||
                bin_node->operator->type == TOKEN_SLASH_ASSIGN ||
                bin_node->operator->type == TOKEN_MODULO_ASSIGN) {

                if (bin_node->left->type != NODE_IDENTIFIER_EXPRESSION) {
                    return ton_error(TON_ERR_RUNTIME, "Invalid assignment target.", node->line, node->column, __FILE__);
                }
                IdentifierExpressionNode* ident_node = (IdentifierExpressionNode*)bin_node->left;
                
                Value right_val;
                TonError err = interpret_expression(bin_node->right, env, &right_val);
                if (err.code != TON_OK) return err;

                if (bin_node->operator->type != TOKEN_ASSIGN) {
                    Value* left_val_ptr = env_get_variable(env, ident_node->identifier);
                    if (!left_val_ptr) {
                        char error_msg[256];
                        snprintf(error_msg, sizeof(error_msg), "Variable '%s' is not defined.", ident_node->identifier);
                        value_release(&right_val);
                        return ton_error(TON_ERR_RUNTIME, error_msg, node->line, node->column, __FILE__);
                    }
                    Value left_val = *left_val_ptr;
                    
                    Value new_val;

                    switch (bin_node->operator->type) {
                        case TOKEN_PLUS_ASSIGN:
                            if (left_val.type == VALUE_INT && right_val.type == VALUE_INT) {
                                new_val = create_value_int(left_val.data.int_val + right_val.data.int_val);
                            } else if (left_val.type == VALUE_FLOAT && right_val.type == VALUE_FLOAT) {
                                new_val = create_value_float(left_val.data.float_val + right_val.data.float_val);
                            } else {
                                value_release(&right_val);
                                return ton_error(TON_ERR_TYPE, "Unsupported types for +=", node->line, node->column, __FILE__);
                            }
                            break;
                        case TOKEN_MINUS_ASSIGN:
                            if (left_val.type == VALUE_INT && right_val.type == VALUE_INT) {
                                new_val = create_value_int(left_val.data.int_val - right_val.data.int_val);
                            } else if (left_val.type == VALUE_FLOAT && right_val.type == VALUE_FLOAT) {
                                new_val = create_value_float(left_val.data.float_val - right_val.data.float_val);
                            } else {
                                value_release(&right_val);
                                return ton_error(TON_ERR_TYPE, "Unsupported types for -=", node->line, node->column, __FILE__);
                            }
                            break;
                        case TOKEN_STAR_ASSIGN:
                            if (left_val.type == VALUE_INT && right_val.type == VALUE_INT) {
                                new_val = create_value_int(left_val.data.int_val * right_val.data.int_val);
                            } else if (left_val.type == VALUE_FLOAT && right_val.type == VALUE_FLOAT) {
                                new_val = create_value_float(left_val.data.float_val * right_val.data.float_val);
                            } else {
                                value_release(&right_val);
                                return ton_error(TON_ERR_TYPE, "Unsupported types for *=", node->line, node->column, __FILE__);
                            }
                            break;
                        case TOKEN_SLASH_ASSIGN:
                            if (left_val.type == VALUE_INT && right_val.type == VALUE_INT) {
                                if (right_val.data.int_val == 0) return ton_error(TON_ERR_RUNTIME, "Division by zero", node->line, node->column, __FILE__);
                                new_val = create_value_int(left_val.data.int_val / right_val.data.int_val);
                            } else if (left_val.type == VALUE_FLOAT && right_val.type == VALUE_FLOAT) {
                                if (right_val.data.float_val == 0.0) return ton_error(TON_ERR_RUNTIME, "Division by zero", node->line, node->column, __FILE__);
                                new_val = create_value_float(left_val.data.float_val / right_val.data.float_val);
                            } else {
                                value_release(&right_val);
                                return ton_error(TON_ERR_TYPE, "Unsupported types for /=", node->line, node->column, __FILE__);
                            }
                            break;
                        case TOKEN_MODULO_ASSIGN:
                             if (left_val.type == VALUE_INT && right_val.type == VALUE_INT) {
                                if (right_val.data.int_val == 0) return ton_error(TON_ERR_RUNTIME, "Division by zero", node->line, node->column, __FILE__);
                                new_val = create_value_int(left_val.data.int_val % right_val.data.int_val);
                            } else {
                                value_release(&right_val);
                                return ton_error(TON_ERR_TYPE, "Unsupported types for %=", node->line, node->column, __FILE__);
                            }
                            break;
                        default: // Should not happen
                            value_release(&right_val);
                            return ton_error(TON_ERR_RUNTIME, "Unhandled compound assignment", node->line, node->column, __FILE__);
                    }
                    value_release(&right_val);
                    right_val = new_val;
                }

                if (!env_set_variable(env, ident_node->identifier, right_val)) {
                    char error_msg[256];
                    snprintf(error_msg, sizeof(error_msg), "Variable '%s' is not defined.", ident_node->identifier);
                    value_release(&right_val);
                    return ton_error(TON_ERR_RUNTIME, error_msg, node->line, node->column, __FILE__);
                }
                *out_result = right_val;
                return ton_ok();
            }

            Value left_val;
            TonError err = interpret_expression(bin_node->left, env, &left_val);
            if (err.code != TON_OK) return err;

            Value right_val;
            err = interpret_expression(bin_node->right, env, &right_val);
            if (err.code != TON_OK) {
                value_release(&left_val);
                return err;
            }

            switch (bin_node->operator->type) {
                case TOKEN_PLUS:
                    if (left_val.type == VALUE_INT && right_val.type == VALUE_INT) {
                        *out_result = create_value_int(left_val.data.int_val + right_val.data.int_val);
                    } else if (left_val.type == VALUE_FLOAT && right_val.type == VALUE_FLOAT) {
                        *out_result = create_value_float(left_val.data.float_val + right_val.data.float_val);
                    } else if (left_val.type == VALUE_STRING && right_val.type == VALUE_STRING) {
                        size_t len = strlen(left_val.data.string_val) + strlen(right_val.data.string_val) + 1;
                        char* concat = ton_malloc(len);
                        if (!concat) {
                            value_release(&left_val);
                            value_release(&right_val);
                            return ton_error(TON_ERR_MEMORY, "Malloc failed for string concat", node->line, node->column, __FILE__);
                        }
                        snprintf(concat, len, "%s%s", left_val.data.string_val, right_val.data.string_val);
                        *out_result = create_value_string(concat);
                        ton_free(concat);
                    } else {
                        value_release(&left_val);
                        value_release(&right_val);
                        return ton_error(TON_ERR_TYPE, "Unsupported types for +", node->line, node->column, __FILE__);
                    }
                    break;
                case TOKEN_MINUS:
                    if (left_val.type == VALUE_INT && right_val.type == VALUE_INT) {
                        *out_result = create_value_int(left_val.data.int_val - right_val.data.int_val);
                    } else if (left_val.type == VALUE_FLOAT && right_val.type == VALUE_FLOAT) {
                        *out_result = create_value_float(left_val.data.float_val - right_val.data.float_val);
                    } else {
                        value_release(&left_val);
                        value_release(&right_val);
                        return ton_error(TON_ERR_TYPE, "Unsupported types for -", node->line, node->column, __FILE__);
                    }
                    break;
                case TOKEN_STAR: // Changed from TOKEN_MULTIPLY
                    if (left_val.type == VALUE_INT && right_val.type == VALUE_INT) {
                        *out_result = create_value_int(left_val.data.int_val * right_val.data.int_val);
                    } else if (left_val.type == VALUE_FLOAT && right_val.type == VALUE_FLOAT) {
                        *out_result = create_value_float(left_val.data.float_val * right_val.data.float_val);
                    } else {
                        value_release(&left_val);
                        value_release(&right_val);
                        return ton_error(TON_ERR_TYPE, "Unsupported types for *", node->line, node->column, __FILE__);
                    }
                    break;
                case TOKEN_SLASH: // Changed from TOKEN_DIVIDE
                    if (left_val.type == VALUE_INT && right_val.type == VALUE_INT) {
                        if (right_val.data.int_val == 0) return ton_error(TON_ERR_RUNTIME, "Division by zero", node->line, node->column, __FILE__);
                        *out_result = create_value_int(left_val.data.int_val / right_val.data.int_val);
                    } else if (left_val.type == VALUE_FLOAT && right_val.type == VALUE_FLOAT) {
                        if (right_val.data.float_val == 0.0) return ton_error(TON_ERR_RUNTIME, "Division by zero", node->line, node->column, __FILE__);
                        *out_result = create_value_float(left_val.data.float_val / right_val.data.float_val);
                    } else {
                        value_release(&left_val);
                        value_release(&right_val);
                        return ton_error(TON_ERR_TYPE, "Unsupported types for /", node->line, node->column, __FILE__);
                    }
                    break;
                case TOKEN_EQ:
                     if (left_val.type != right_val.type) *out_result = create_value_bool(0);
                     else {
                         int eq = 0;
                         switch (left_val.type) {
                             case VALUE_INT: eq = left_val.data.int_val == right_val.data.int_val; break;
                             case VALUE_FLOAT: eq = left_val.data.float_val == right_val.data.float_val; break;
                             case VALUE_BOOL: eq = left_val.data.bool_val == right_val.data.bool_val; break;
                             case VALUE_STRING: eq = strcmp(left_val.data.string_val, right_val.data.string_val) == 0; break;
                             default: return ton_error(TON_ERR_TYPE, "Unsupported types for ==", node->line, node->column, __FILE__);
                         }
                         *out_result = create_value_bool(eq);
                     }
                     break;
                case TOKEN_NEQ: // Changed from TOKEN_NE
                     if (left_val.type != right_val.type) *out_result = create_value_bool(1);
                     else {
                         int neq = 0;
                         switch (left_val.type) {
                             case VALUE_INT: neq = left_val.data.int_val != right_val.data.int_val; break;
                             case VALUE_FLOAT: neq = left_val.data.float_val != right_val.data.float_val; break;
                             case VALUE_BOOL: neq = left_val.data.bool_val != right_val.data.bool_val; break;
                             case VALUE_STRING: neq = strcmp(left_val.data.string_val, right_val.data.string_val) != 0; break;
                             default: return ton_error(TON_ERR_TYPE, "Unsupported types for !=", node->line, node->column, __FILE__);
                         }
                         *out_result = create_value_bool(neq);
                     }
                     break;
                case TOKEN_LT:
                case TOKEN_LE:
                case TOKEN_GT:
                case TOKEN_GE:
                    if (left_val.type == VALUE_INT && right_val.type == VALUE_INT) {
                        int cmp = 0;
                        if (bin_node->operator->type == TOKEN_LT) cmp = left_val.data.int_val < right_val.data.int_val;
                        else if (bin_node->operator->type == TOKEN_LE) cmp = left_val.data.int_val <= right_val.data.int_val;
                        else if (bin_node->operator->type == TOKEN_GT) cmp = left_val.data.int_val > right_val.data.int_val;
                        else cmp = left_val.data.int_val >= right_val.data.int_val;
                        *out_result = create_value_bool(cmp);
                    } else if (left_val.type == VALUE_FLOAT && right_val.type == VALUE_FLOAT) {
                        int cmp = 0;
                        if (bin_node->operator->type == TOKEN_LT) cmp = left_val.data.float_val < right_val.data.float_val;
                        else if (bin_node->operator->type == TOKEN_LE) cmp = left_val.data.float_val <= right_val.data.float_val;
                        else if (bin_node->operator->type == TOKEN_GT) cmp = left_val.data.float_val > right_val.data.float_val;
                        else cmp = left_val.data.float_val >= right_val.data.float_val;
                        *out_result = create_value_bool(cmp);
                    } else {
                        value_release(&left_val);
                        value_release(&right_val);
                        return ton_error(TON_ERR_TYPE, "Unsupported types for comparison", node->line, node->column, __FILE__);
                    }
                    break;
                default:
                    value_release(&left_val);
                    value_release(&right_val);
                    return ton_error(TON_ERR_RUNTIME, "Unsupported binary operator", node->line, node->column, __FILE__);
            }
            value_release(&left_val);
            value_release(&right_val);
            return ton_ok();
        }
        case NODE_UNARY_EXPRESSION: {
            UnaryExpressionNode* unary = (UnaryExpressionNode*)node;

            if (unary->operator->type == TOKEN_INCREMENT || unary->operator->type == TOKEN_DECREMENT) {
                if (unary->operand->type != NODE_IDENTIFIER_EXPRESSION) {
                    return ton_error(TON_ERR_RUNTIME, "Operand of increment/decrement must be an identifier.", node->line, node->column, __FILE__);
                }
                IdentifierExpressionNode* ident_node = (IdentifierExpressionNode*)unary->operand;
                Value* var_val = env_get_variable(env, ident_node->identifier);
                if (!var_val) {
                    char error_msg[256];
                    snprintf(error_msg, sizeof(error_msg), "Variable '%s' is not defined.", ident_node->identifier);
                    return ton_error(TON_ERR_RUNTIME, error_msg, node->line, node->column, __FILE__);
                }

                if (var_val->type == VALUE_INT) {
                    int original_val = var_val->data.int_val;
                    if (unary->operator->type == TOKEN_INCREMENT) {
                        var_val->data.int_val++;
                    } else {
                        var_val->data.int_val--;
                    }
                    if (unary->is_postfix) {
                        *out_result = create_value_int(original_val);
                    } else {
                        *out_result = create_value_int(var_val->data.int_val);
                        value_add_ref(out_result);
                    }
                } else if (var_val->type == VALUE_FLOAT) {
                    float original_val = var_val->data.float_val;
                    if (unary->operator->type == TOKEN_INCREMENT) {
                        var_val->data.float_val++;
                    } else {
                        var_val->data.float_val--;
                    }
                    if (unary->is_postfix) {
                        *out_result = create_value_float(original_val);
                    } else {
                        *out_result = create_value_float(var_val->data.float_val);
                        value_add_ref(out_result);
                    }
                } else {
                    return ton_error(TON_ERR_TYPE, "Operand of increment/decrement must be a number.", node->line, node->column, __FILE__);
                }
                return ton_ok();
            }

            Value operand;
            TonError err = interpret_expression(unary->operand, env, &operand);
            if (err.code != TON_OK) return err;

            switch (unary->operator->type) {
                case TOKEN_MINUS:
                    if (operand.type == VALUE_INT) {
                        *out_result = create_value_int(-operand.data.int_val);
                    } else if (operand.type == VALUE_FLOAT) {
                        *out_result = create_value_float(-operand.data.float_val);
                    } else {
                        value_release(&operand);
                        return ton_error(TON_ERR_TYPE, "Negation only for int/float", node->line, node->column, __FILE__);
                    }
                    break;
                case TOKEN_NOT:
                    if (operand.type == VALUE_BOOL) {
                        *out_result = create_value_bool(!operand.data.bool_val);
                    } else {
                        value_release(&operand);
                        return ton_error(TON_ERR_TYPE, "NOT only for bool", node->line, node->column, __FILE__);
                    }
                    break;
                default:
                    value_release(&operand);
                    return ton_error(TON_ERR_RUNTIME, "Unsupported unary operator", node->line, node->column, __FILE__);
            }
            value_release(&operand);
            return ton_ok();
        }
        case NODE_TYPEOF_EXPRESSION: {
            TypeofExpressionNode* typeof_node = (TypeofExpressionNode*)node;
            Value operand;
            TonError err = interpret_expression(typeof_node->operand, env, &operand);
            if (err.code != TON_OK) return err;
            const char* type_str = value_type_to_string(operand.type);
            *out_result = create_value_string(type_str);
            value_release(&operand);
            return ton_ok();
        }
        case NODE_MEMBER_ACCESS_EXPRESSION: {
            MemberAccessExpressionNode* member_node = (MemberAccessExpressionNode*)node;
            Value object_val;
            TonError err = interpret_expression(member_node->object, env, &object_val);
            if (err.code != TON_OK) return err;

            if (object_val.type != VALUE_STRUCT) {
                value_release(&object_val);
                return ton_error(TON_ERR_TYPE, "Member access operator (.) can only be used on structs.", node->line, node->column, __FILE__);
            }

            TonStructInstance* instance = (TonStructInstance*)object_val.data.struct_val;
            *out_result = struct_get_field(instance, member_node->member);
            value_release(&object_val);
            return ton_ok();
        }
        case NODE_METHOD_CALL_EXPRESSION: {
            printf("Node type: %d\n", node->type);
            if (node->type == NODE_METHOD_CALL_EXPRESSION) {
                // Use direct field access through pointer arithmetic to avoid compilation issues
                ASTNode* object = *(ASTNode**)((char*)node + sizeof(ASTNode));
                char* method_name = *(char**)((char*)node + sizeof(ASTNode) + sizeof(ASTNode*));
                // ASTNode** arguments and int num_arguments are commented out for now
                
            Value object_val;
            TonError err = interpret_expression(object, env, &object_val);
            if (err.code != TON_OK) return err;

            if (object_val.type != VALUE_STRUCT) {
                value_release(&object_val);
                return ton_error(TON_ERR_TYPE, "Method call operator (.) can only be used on structs.", node->line, node->column, __FILE__);
            }

            TonStructInstance* instance = (TonStructInstance*)object_val.data.struct_val;
            FunctionDeclarationNode* method = struct_get_method(instance->type, method_name);

            if (!method) {
                value_release(&object_val);
                char error_msg[256];
                snprintf(error_msg, sizeof(error_msg), "Method '%s' not found in struct '%s'.", method_name, instance->type->name);
                return ton_error(TON_ERR_RUNTIME, error_msg, node->line, node->column, __FILE__);
            }

            Environment* method_env = create_child_environment(env);
            env_add_variable(method_env, "this", object_val, value_type_to_variable_type(object_val.type));

            /* Argument checking and processing commented out for now */

                // Execute the method body
                Value result;
                err = interpret_statement((ASTNode*)method->body, method_env, &result);
                if (err.code != TON_OK && err.code != TON_RETURN) {
                    value_release(&object_val);
                    env_release(method_env);
                    return err;
                }

                // Get the return value
                if (err.code == TON_RETURN) {
                    *out_result = result;
                } else {
                    *out_result = create_value_null();
                }

                value_release(&object_val);
                env_release(method_env);
                return ton_ok();
            }
            return ton_error(TON_ERR_RUNTIME, "Invalid node type in method call expression", node->line, node->column, __FILE__);
        }
        case NODE_SIZEOF_EXPRESSION: {
            SizeofExpressionNode* sizeof_node = (SizeofExpressionNode*)node;
            Value operand;
            TonError err = interpret_expression(sizeof_node->operand, env, &operand);
            if (err.code != TON_OK) return err;
            size_t size = 0;
            switch (operand.type) {
                case VALUE_INT: size = sizeof(int); break;
                case VALUE_FLOAT: size = sizeof(float); break;
                case VALUE_BOOL: size = sizeof(bool); break;
                case VALUE_CHAR: size = sizeof(char); break;
                case VALUE_STRING: size = strlen(operand.data.string_val) + 1; break;
                case VALUE_ARRAY: size = sizeof(TonArray); break; // Placeholder, actual size depends on elements
                case VALUE_STRUCT: size = sizeof(TonStructInstance); break; // Placeholder
                default: size = 0; break;
            }
            *out_result = create_value_int(size);
            value_release(&operand);
            return ton_ok();
        }
        case NODE_ALIGNOF_EXPRESSION: {
            AlignofExpressionNode* alignof_node = (AlignofExpressionNode*)node;
            Value operand;
            TonError err = interpret_expression(alignof_node->operand, env, &operand);
            if (err.code != TON_OK) return err;
            size_t alignment = 0;
            switch (operand.type) {
                case VALUE_INT: alignment = _Alignof(int); break;
                case VALUE_FLOAT: alignment = _Alignof(float); break;
                case VALUE_BOOL: alignment = _Alignof(bool); break;
                case VALUE_CHAR: alignment = _Alignof(char); break;
                case VALUE_STRING: alignment = _Alignof(char*); break;
                case VALUE_ARRAY: alignment = _Alignof(TonArray*); break;
                case VALUE_STRUCT: alignment = _Alignof(TonStructInstance*); break;
                default: alignment = 0; break;
            }
            *out_result = create_value_int(alignment);
            value_release(&operand);
            return ton_ok();
        }
        case NODE_NEW_EXPRESSION: {
            NewExpressionNode* new_node = (NewExpressionNode*)node;
            TonStructType* struct_type = find_struct_type(new_node->class_name);
            if (!struct_type) {
                char error_msg[256];
                snprintf(error_msg, sizeof(error_msg), "Struct type '%s' not found.", new_node->class_name);
                return ton_error(TON_ERR_RUNTIME, error_msg, node->line, node->column, __FILE__);
            }

            TonStructInstance* instance = create_struct_instance(struct_type);
            if (!instance) {
                return ton_error(TON_ERR_RUNTIME, "Failed to create struct instance.", node->line, node->column, __FILE__);
            }

            for (int i = 0; i < new_node->num_arguments; ++i) {
                ASTNode* arg_node = new_node->arguments[i];
                if (arg_node->type != NODE_BINARY_EXPRESSION || ((BinaryExpressionNode*)arg_node)->operator->type != TOKEN_COLON) {
                    destroy_struct_instance(instance);
                    return ton_error(TON_ERR_SYNTAX, "Invalid syntax for struct instantiation. Expected 'field: value'.", node->line, node->column, __FILE__);
                }

                BinaryExpressionNode* field_init_node = (BinaryExpressionNode*)arg_node;
                if (field_init_node->left->type != NODE_IDENTIFIER_EXPRESSION) {
                    destroy_struct_instance(instance);
                    return ton_error(TON_ERR_SYNTAX, "Expected field name in struct instantiation.", node->line, node->column, __FILE__);
                }

                char* field_name = ((IdentifierExpressionNode*)field_init_node->left)->identifier;
                Value field_value;
                TonError err = interpret_expression(field_init_node->right, env, &field_value);
                if (err.code != TON_OK) {
                    destroy_struct_instance(instance);
                    return err;
                }

                if (!struct_set_field(instance, field_name, field_value)) {
                    char error_msg[256];
                    snprintf(error_msg, sizeof(error_msg), "Field '%s' not found in struct '%s'.", field_name, new_node->class_name);
                    destroy_struct_instance(instance);
                    value_release(&field_value);
                    return ton_error(TON_ERR_RUNTIME, error_msg, node->line, node->column, __FILE__);
                }
                value_release(&field_value);
            }

            *out_result = create_value_struct(instance);
            return ton_ok();
        }
        case NODE_ARRAY_LITERAL_EXPRESSION: {
            ArrayLiteralExpressionNode* array_lit = (ArrayLiteralExpressionNode*)node;
            TonArray* arr = create_dynamic_array(0);
            if (!arr) {
                return ton_error(TON_ERR_MEMORY, "Failed to create array", node->line, node->column, __FILE__);
            }

            for (int i = 0; i < array_lit->num_elements; i++) {
                Value element_val;
                TonError err = interpret_expression(array_lit->elements[i], env, &element_val);
                if (err.code != TON_OK) {
                    destroy_array(arr);
                    return err;
                }
                array_push(arr, element_val);
            }
            *out_result = create_value_array(arr);
            return ton_ok();
        }
        case NODE_MACRO_CALL_EXPRESSION: {
            MacroCallExpressionNode* macro_call = (MacroCallExpressionNode*)node;
            *out_result = evaluate_macro_call_expression(macro_call, env);
            return ton_ok();
        }
         default:
             return ton_error(TON_ERR_RUNTIME, "Unsupported expression type", node->line, node->column, __FILE__);
    }
}