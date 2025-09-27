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

TonError interpret_expression(ASTNode* node, Environment* env, Value* out_result) {
    if (!node || !env || !out_result) {
        return ton_error(TON_ERR_RUNTIME, "Invalid arguments to interpret_expression");
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
            } else {
                return ton_error(TON_ERR_RUNTIME, "Unsupported literal type");
            }
            return ton_ok();
        }
        case NODE_IDENTIFIER_EXPRESSION: {
            IdentifierExpressionNode* id_expr = (IdentifierExpressionNode*)node;
            Value* var = env_get_variable(env, id_expr->identifier);
            if (var) {
                *out_result = *var;
                value_add_ref(out_result);
            } else {
                 char error_msg[256];
                 snprintf(error_msg, sizeof(error_msg), "Variable '%s' is not defined.", id_expr->identifier);
                 return ton_error(TON_ERR_RUNTIME, error_msg);
            }
            return ton_ok();
        }
        case NODE_FN_CALL_EXPRESSION: {
            FunctionCallExpressionNode* call_node = (FunctionCallExpressionNode*)node;
            Value callee_val;
            TonError err = interpret_expression(call_node->callee, env, &callee_val);
            if (err.code != TON_OK) return err;

            if (callee_val.type != VALUE_FN) {
                value_release(&callee_val);
                return ton_error(TON_ERR_TYPE, "Cannot call non-function value");
            }

            Function* function = callee_val.data.function_value;
            value_release(&callee_val);

            if (call_node->num_arguments != function->num_parameters) {
                return ton_error(TON_ERR_TYPE, "Argument count mismatch");
            }

            Value* args = (Value*)ton_malloc(sizeof(Value) * call_node->num_arguments);
            if (!args) {
                return ton_error(TON_ERR_MEMORY, "Memory allocation failed for arguments");
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
            if (bin_node->operator->type == TOKEN_ASSIGN) {
                if (bin_node->left->type != NODE_IDENTIFIER_EXPRESSION) {
                    return ton_error(TON_ERR_RUNTIME, "Invalid assignment target.");
                }
                IdentifierExpressionNode* ident_node = (IdentifierExpressionNode*)bin_node->left;
                Value right_val;
                TonError err = interpret_expression(bin_node->right, env, &right_val);
                if (err.code != TON_OK) return err;

                if (!env_set_variable(env, ident_node->identifier, right_val)) {
                    char error_msg[256];
                    snprintf(error_msg, sizeof(error_msg), "Variable '%s' is not defined.", ident_node->identifier);
                    value_release(&right_val);
                    return ton_error(TON_ERR_RUNTIME, error_msg);
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
                            return ton_error(TON_ERR_MEMORY, "Malloc failed for string concat");
                        }
                        snprintf(concat, len, "%s%s", left_val.data.string_val, right_val.data.string_val);
                        *out_result = create_value_string(concat);
                        ton_free(concat);
                    } else {
                        value_release(&left_val);
                        value_release(&right_val);
                        return ton_error(TON_ERR_TYPE, "Unsupported types for +");
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
                        return ton_error(TON_ERR_TYPE, "Unsupported types for -");
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
                        return ton_error(TON_ERR_TYPE, "Unsupported types for *");
                    }
                    break;
                case TOKEN_SLASH: // Changed from TOKEN_DIVIDE
                    if (left_val.type == VALUE_INT && right_val.type == VALUE_INT) {
                        if (right_val.data.int_val == 0) return ton_error(TON_ERR_RUNTIME, "Division by zero");
                        *out_result = create_value_int(left_val.data.int_val / right_val.data.int_val);
                    } else if (left_val.type == VALUE_FLOAT && right_val.type == VALUE_FLOAT) {
                        if (right_val.data.float_val == 0.0) return ton_error(TON_ERR_RUNTIME, "Division by zero");
                        *out_result = create_value_float(left_val.data.float_val / right_val.data.float_val);
                    } else {
                        value_release(&left_val);
                        value_release(&right_val);
                        return ton_error(TON_ERR_TYPE, "Unsupported types for /");
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
                             default: return ton_error(TON_ERR_TYPE, "Unsupported types for ==");
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
                             default: return ton_error(TON_ERR_TYPE, "Unsupported types for !=");
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
                        return ton_error(TON_ERR_TYPE, "Unsupported types for comparison");
                    }
                    break;
                default:
                    value_release(&left_val);
                    value_release(&right_val);
                    return ton_error(TON_ERR_RUNTIME, "Unsupported binary operator");
            }
            value_release(&left_val);
            value_release(&right_val);
            return ton_ok();
        }
        case NODE_UNARY_EXPRESSION: {
            UnaryExpressionNode* unary = (UnaryExpressionNode*)node;
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
                        return ton_error(TON_ERR_TYPE, "Negation only for int/float");
                    }
                    break;
                case TOKEN_NOT:
                    if (operand.type == VALUE_BOOL) {
                        *out_result = create_value_bool(!operand.data.bool_val);
                    } else {
                        value_release(&operand);
                        return ton_error(TON_ERR_TYPE, "NOT only for bool");
                    }
                    break;
                default:
                    value_release(&operand);
                    return ton_error(TON_ERR_RUNTIME, "Unsupported unary operator");
            }
            value_release(&operand);
            return ton_ok();
        }
         default:
             return ton_error(TON_ERR_RUNTIME, "Unsupported expression type");
    }
}