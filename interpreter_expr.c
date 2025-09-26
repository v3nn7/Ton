#include <string.h>
#include "array.h"
#include "interpreter_expr.h"
#include "ast.h"
#include "environment.h"
#include "value.h"
#include "token.h"



#include "interpreter_stmt.h"

Value interpret_expression(ASTNode* node, Environment* env) {
    switch (node->type) {
        case NODE_LITERAL_EXPRESSION:
            {
                LiteralExpressionNode* lit_expr = (LiteralExpressionNode*)node;
                if (lit_expr->value->type == TOKEN_INT_LITERAL) {
                    return create_value_int(atoi(lit_expr->value->lexeme));
                } else if (lit_expr->value->type == TOKEN_STRING_LITERAL) {
                    // Remove quotes from the string literal
                    char* temp_str = strdup(lit_expr->value->lexeme);
                    temp_str[strlen(temp_str) - 1] = '\0'; // Remove closing quote
                    Value result = create_value_string(temp_str + 1); // Skip opening quote
                    free(temp_str); // Free the duplicated string
                    return result;
                } else if (lit_expr->value->type == TOKEN_TRUE || lit_expr->value->type == TOKEN_FALSE) {
                    return create_value_int(lit_expr->value->type == TOKEN_TRUE ? 1 : 0);
                } else if (lit_expr->value->type == TOKEN_FLOAT_LITERAL) {
                    return create_value_float(atof(lit_expr->value->lexeme));
                } else {
                    // return runtime_error("Only integer, boolean, float and string literals are supported for now.");
                    return create_value_int(0);
                }
            }
        case NODE_IDENTIFIER_EXPRESSION: {
            IdentifierExpressionNode* id_expr = (IdentifierExpressionNode*)node;
            // Get the Value directly from environment
            Value value = env_get_variable(env, id_expr->identifier);
            return value;
        }
        case NODE_FN_CALL_EXPRESSION: {
            FunctionCallExpressionNode* call_node = (FunctionCallExpressionNode*)node;
            Value callee_val = interpret_expression(call_node->callee, env);

            if (callee_val.type != VALUE_FN) {
                // return runtime_error("Cannot call non-function value.");
                return create_value_int(0);
            }

            Function* function = callee_val.data.function_value;

            if (call_node->num_arguments != function->num_parameters) {
                /* return runtime_error("Function '%s' expects %d arguments, but got %d.",
                                     function->name, function->num_parameters, call_node->num_arguments);
                */
                return create_value_int(0);
            }

            Value* args = (Value*)ton_malloc(sizeof(Value) * call_node->num_arguments);
            if (!args) {
                // return runtime_error("Memory allocation failed for arguments.");
                return create_value_int(0);
            }

            for (int i = 0; i < call_node->num_arguments; i++) {
                args[i] = interpret_expression(call_node->arguments[i], env);
            }

            if (strcmp(function->name, "read_line") == 0) {
                // Value result = builtin_read_line(args, call_node->num_arguments);
                ton_free(args);
                // return result;
                return create_value_int(0);
            } else {
                // User-defined function
                Environment* fn_env = create_child_environment(function->closure_env);
                for (int i = 0; i < function->num_parameters; i++) {
                    ParameterNode* param = function->parameters[i];
                    env_add_variable(fn_env, param->identifier->lexeme, args[i]);
                }
                Value result = interpret_statement(function->body, fn_env);
                destroy_environment(fn_env);
                ton_free(args);
                return result;
            }
            break;
        }
        case NODE_CONDITIONAL_EXPRESSION: {
            ConditionalExpressionNode* cond_expr = (ConditionalExpressionNode*)node;
            Value condition = interpret_expression(cond_expr->condition, env);

            if (condition.type != VALUE_BOOL) {
                // return runtime_error("Conditional expression must be a boolean.");
                return create_value_int(0);
            }

            if (condition.data.bool_val) {
                return interpret_expression(cond_expr->true_expr, env);
            } else {
                return interpret_expression(cond_expr->false_expr, env);
            }
        }
        case NODE_TYPEOF_EXPRESSION: {
            TypeofExpressionNode* typeof_expr = (TypeofExpressionNode*)node;
            Value operand_value = interpret_expression(typeof_expr->operand, env);
            
            Value result = create_value_string((char*)value_type_to_string(operand_value.type));
            
            value_release(&operand_value);
            
            return result;
        }
        case NODE_ARRAY_LITERAL_EXPRESSION: {
            ArrayLiteralExpressionNode* array_lit = (ArrayLiteralExpressionNode*)node;
            
            // Create a dynamic array and populate it with elements
            TonArray* arr = create_dynamic_array(array_lit->num_elements);
            if (!arr) {
                // return runtime_error("Failed to create array.");
                return create_value_int(0);
            }
            
            for (size_t i = 0; i < (size_t)array_lit->num_elements; i++) {
                Value element = interpret_expression(array_lit->elements[i], env);
                if (!array_push(arr, element)) {
                    value_release(&element);
                    destroy_array(arr);
                    // return runtime_error("Failed to add element to array.");
                    return create_value_int(0);
                }
            }
            
            return create_value_array(arr);
        }
        case NODE_ARRAY_ACCESS_EXPRESSION: {
            ArrayAccessExpressionNode* array_access = (ArrayAccessExpressionNode*)node;
            
            // Get the array value
            Value array_val = interpret_expression(array_access->array, env);
            if (array_val.type != VALUE_ARRAY) {
                value_release(&array_val);
                // return runtime_error("Cannot index non-array value.");
                return create_value_int(0);
            }
            
            // Get the index value
            Value index_val = interpret_expression(array_access->index, env);
            if (index_val.type != VALUE_INT) {
                value_release(&array_val);
                value_release(&index_val);
                // return runtime_error("Array index must be an integer.");
                return create_value_int(0);
            }
            
            TonArray* arr = (TonArray*)array_val.data.array_val;
            size_t index = (size_t)index_val.data.int_val;
            
            if (index >= arr->length) {
                value_release(&array_val);
                value_release(&index_val);
                // return runtime_error("Array index out of bounds.");
                return create_value_int(0);
            }
            
            Value result = array_get(arr, index);
            value_release(&array_val);
            value_release(&index_val);
            return result;
        }
        case NODE_BINARY_EXPRESSION: {
            BinaryExpressionNode* bin_op = (BinaryExpressionNode*)node;
            Value left = interpret_expression(bin_op->left, env);
            Value right = interpret_expression(bin_op->right, env);

            Value result;
            switch (bin_op->operator->type) {
                // Arithmetic operators
                case TOKEN_PLUS:
                    if (left.type == VALUE_INT && right.type == VALUE_INT) {
                        result = create_value_int(left.data.int_val + right.data.int_val);
                    } else if (left.type == VALUE_FLOAT && right.type == VALUE_FLOAT) {
                        result = create_value_float(left.data.float_val + right.data.float_val);
                    } else if (left.type == VALUE_STRING && right.type == VALUE_STRING) {
                        size_t len = strlen(left.data.string_val) + strlen(right.data.string_val) + 1;
                        char* concat = ton_malloc(len);
                        snprintf(concat, len, "%s%s", left.data.string_val, right.data.string_val);
                        result = create_value_string(concat);
                        ton_free(concat);
                    } else {
                        runtime_error("Unsupported types for + operator");
                        result = create_value_int(0);
                    }
                    break;
                case TOKEN_MINUS:
                    if (left.type == VALUE_INT && right.type == VALUE_INT) {
                        result = create_value_int(left.data.int_val - right.data.int_val);
                    } else if (left.type == VALUE_FLOAT && right.type == VALUE_FLOAT) {
                        result = create_value_float(left.data.float_val - right.data.float_val);
                    } else {
                        runtime_error("Unsupported types for - operator");
                        result = create_value_int(0);
                    }
                    break;
                case TOKEN_STAR:
                    if (left.type == VALUE_INT && right.type == VALUE_INT) {
                        result = create_value_int(left.data.int_val * right.data.int_val);
                    } else if (left.type == VALUE_FLOAT && right.type == VALUE_FLOAT) {
                        result = create_value_float(left.data.float_val * right.data.float_val);
                    } else {
                        runtime_error("Unsupported types for * operator");
                        result = create_value_int(0);
                    }
                    break;
                case TOKEN_SLASH:
                    if (left.type == VALUE_INT && right.type == VALUE_INT) {
                        if (right.data.int_val == 0) {
                            runtime_error("Division by zero");
                            result = create_value_int(0);
                        } else {
                            result = create_value_int(left.data.int_val / right.data.int_val);
                        }
                    } else if (left.type == VALUE_FLOAT && right.type == VALUE_FLOAT) {
                        if (right.data.float_val == 0.0) {
                            runtime_error("Division by zero");
                            result = create_value_float(0.0);
                        } else {
                            result = create_value_float(left.data.float_val / right.data.float_val);
                        }
                    } else {
                        runtime_error("Unsupported types for / operator");
                        result = create_value_int(0);
                    }
                    break;
                // Comparison operators
                case TOKEN_EQ:
                    if (left.type == right.type) {
                        int eq = 0;
                        switch (left.type) {
                            case VALUE_INT: eq = left.data.int_val == right.data.int_val; break;
                            case VALUE_FLOAT: eq = left.data.float_val == right.data.float_val; break;
                            case VALUE_BOOL: eq = left.data.bool_val == right.data.bool_val; break;
                            case VALUE_STRING: eq = strcmp(left.data.string_val, right.data.string_val) == 0; break;
                            default: runtime_error("Unsupported types for == operator"); eq = 0;
                        }
                        result = create_value_bool(eq);
                    } else {
                        result = create_value_bool(0);
                    }
                    break;
                case TOKEN_NEQ:
                    if (left.type == right.type) {
                        int neq = 0;
                        switch (left.type) {
                            case VALUE_INT: neq = left.data.int_val != right.data.int_val; break;
                            case VALUE_FLOAT: neq = left.data.float_val != right.data.float_val; break;
                            case VALUE_BOOL: neq = left.data.bool_val != right.data.bool_val; break;
                            case VALUE_STRING: neq = strcmp(left.data.string_val, right.data.string_val) != 0; break;
                            default: runtime_error("Unsupported types for != operator"); neq = 0;
                        }
                        result = create_value_bool(neq);
                    } else {
                        result = create_value_bool(1);
                    }
                    break;
                case TOKEN_LT:
                case TOKEN_LE:
                case TOKEN_GT:
                case TOKEN_GE:
                    if (left.type == VALUE_INT && right.type == VALUE_INT) {
                        int cmp;
                        if (bin_op->operator->type == TOKEN_LT) cmp = left.data.int_val < right.data.int_val;
                        else if (bin_op->operator->type == TOKEN_LE) cmp = left.data.int_val <= right.data.int_val;
                        else if (bin_op->operator->type == TOKEN_GT) cmp = left.data.int_val > right.data.int_val;
                        else cmp = left.data.int_val >= right.data.int_val;
                        result = create_value_bool(cmp);
                    } else if (left.type == VALUE_FLOAT && right.type == VALUE_FLOAT) {
                        int cmp;
                        if (bin_op->operator->type == TOKEN_LT) cmp = left.data.float_val < right.data.float_val;
                        else if (bin_op->operator->type == TOKEN_LE) cmp = left.data.float_val <= right.data.float_val;
                        else if (bin_op->operator->type == TOKEN_GT) cmp = left.data.float_val > right.data.float_val;
                        else cmp = left.data.float_val >= right.data.float_val;
                        result = create_value_bool(cmp);
                    } else {
                        runtime_error("Unsupported types for comparison operator");
                        result = create_value_int(0);
                    }
                    break;
                // Logical operators
                case TOKEN_AND:
                    result = create_value_bool(left.data.bool_val && right.data.bool_val);
                    break;
                case TOKEN_OR:
                    result = create_value_bool(left.data.bool_val || right.data.bool_val);
                    break;
                default:
                    runtime_error("Unsupported binary operator");
                    result = create_value_int(0);
                    break;
            }

            value_release(&left);
            value_release(&right);
            return result;
        }
        case NODE_UNARY_EXPRESSION: {
            UnaryExpressionNode* unary = (UnaryExpressionNode*)node;
            Value operand = interpret_expression(unary->operand, env);

            switch (unary->operator->type) {
                case TOKEN_MINUS:
                    if (operand.type == VALUE_INT) {
                        return create_value_int(-operand.data.int_val);
                    } else if (operand.type == VALUE_FLOAT) {
                        return create_value_float(-operand.data.float_val);
                    } else {
                        runtime_error("Negation only supported for int and float");
                        value_release(&operand);
                        return create_value_int(0);
                    }
                    break;
                case TOKEN_NOT:
                    if (operand.type == VALUE_BOOL) {
                        return create_value_bool(!operand.data.bool_val);
                    } else {
                        runtime_error("Logical not only supported for bool");
                        value_release(&operand);
                        return create_value_int(0);
                    }
                    break;
                default:
                    runtime_error("Unsupported unary operator");
                    value_release(&operand);
                    return create_value_int(0);
            }
        }
        default:
            runtime_error("Unsupported expression type");
            return create_value_int(0);
    }
}