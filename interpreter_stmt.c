#include <stdio.h>
#include "interpreter_stmt.h"
#include "interpreter.h"
#include "environment.h"
#include "ast.h"
#include "value.h"
#include "builtin.h"
#include "memory.h"

#include "interpreter_expr.h"

Value interpret_statement(ASTNode* node, Environment* env) {
    int break_flag = 0;
    int continue_flag = 0;
    int return_flag = 0;
    Value return_value_holder = create_value_int(0);
    switch (node->type) {
        case NODE_BLOCK_STATEMENT: {
            BlockStatementNode* block_node = (BlockStatementNode*)node;
            Value last_value = create_value_int(0);
            for (int i = 0; i < block_node->num_statements; i++) {
                last_value = interpret_statement(block_node->statements[i], env);
                // Note: a proper implementation would need to handle return flags
            }
            return last_value;
        }
        case NODE_IF_STATEMENT: {
            IfStatementNode* if_stmt = (IfStatementNode*)node;
            Value cond = interpret_expression(if_stmt->condition, env);
            if (cond.type != VALUE_INT && cond.type != VALUE_BOOL) {
                // In a real implementation, you would call a runtime_error function
                return create_value_int(0);
            }
            int condition_result = 0;
            if (cond.type == VALUE_BOOL) {
                condition_result = cond.data.bool_val;
            } else {
                condition_result = cond.data.int_val != 0;  // non-zero means true
            }
            value_release(&cond);
            if (condition_result) {
                return interpret_statement((ASTNode*)if_stmt->consequence, env);
            } else if (if_stmt->alternative) {
                return interpret_statement((ASTNode*)if_stmt->alternative, env);
            } else {
                return create_value_int(0);
            }
        }
        case NODE_WHILE_STATEMENT: {
            WhileStatementNode* wh = (WhileStatementNode*)node;
            for (;;) {
                Value cond = interpret_expression(wh->condition, env);
                int condition_result = 0;
                if (cond.type == VALUE_INT) {
                    condition_result = cond.data.int_val != 0;  // non-zero means true
                } else if (cond.type == VALUE_BOOL) {
                    condition_result = cond.data.bool_val;
                } else {
                    // runtime_error("while condition must evaluate to an integer or boolean.");
                    return create_value_int(0);
                }
                value_release(&cond);
                if (!condition_result) {
                    return create_value_int(0);
                }
                // break_flag, continue_flag, and return_flag handling would be needed here
                Value v = interpret_statement((ASTNode*)wh->body, env);
            }
        }
        case NODE_FOR_STATEMENT: {
            ForStatementNode* for_stmt = (ForStatementNode*)node;
            if (for_stmt->init) {
                interpret_expression(for_stmt->init, env);
            }
            while (1) {
                if (for_stmt->condition) {
                    Value cond = interpret_expression(for_stmt->condition, env);
                    if (cond.type != VALUE_INT) {
                        // runtime_error("for condition must evaluate to an integer (0/1).");
                         return create_value_int(0);
                    }
                    if (cond.data.int_val == 0) {
                        break; // Exit loop
                    }
                }
                // break_flag, continue_flag, and return_flag handling would be needed here
                Value v = interpret_statement((ASTNode*)for_stmt->body, env);
                if (for_stmt->update) {
                    interpret_expression(for_stmt->update, env);
                }
            }
            return create_value_int(0);
        }
        case NODE_LOOP_STATEMENT: {
            LoopStatementNode* loop_stmt = (LoopStatementNode*)node;
            while (1) {
                // break_flag, continue_flag, and return_flag handling would be needed here
                Value v = interpret_statement((ASTNode*)loop_stmt->body, env);
            }
        }
        case NODE_SWITCH_STATEMENT: {
            SwitchStatementNode* switch_stmt = (SwitchStatementNode*)node;
            Value switch_value = interpret_expression(switch_stmt->expression, env);
            
            if (switch_value.type != VALUE_INT) {
                // runtime_error("switch expression must evaluate to an integer.");
                 return create_value_int(0);
            }
            
            int switch_int_val = switch_value.data.int_val;
            value_release(&switch_value);

            // Check each case
            for (int i = 0; i < switch_stmt->num_cases; i++) {
                CaseStatementNode* case_stmt = switch_stmt->cases[i];
                Value case_value = interpret_expression(case_stmt->value, env);
                
                if (case_value.type != VALUE_INT) {
                    // runtime_error("case value must be an integer.");
                     return create_value_int(0);
                }
                
                int case_int_val = case_value.data.int_val;
                value_release(&case_value);

                if (switch_int_val == case_int_val) {
                    // Execute all statements in this case
                    Value last_value = create_value_int(0);
                    for (int j = 0; j < case_stmt->num_statements; j++) {
                        last_value = interpret_statement(case_stmt->statements[j], env);
                        // if (return_flag) return last_value;
                    }
                    return last_value;
                }
            }
            
            // If no case matched, execute default case if it exists
            if (switch_stmt->default_case) {
                return interpret_statement((ASTNode*)switch_stmt->default_case, env);
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
            Value tmp = interpret_expression(expr_stmt->expression, env);
            value_release(&tmp);
            // Expression statements do not yield a meaningful value
            return create_value_int(0);
        }
        case NODE_RETURN_STATEMENT: {
            ReturnStatementNode* ret_stmt = (ReturnStatementNode*)node;
            Value return_value = interpret_expression(ret_stmt->expression, env);
            program_exit_code = (return_value.type == VALUE_INT) ? return_value.data.int_val : program_exit_code; // Set exit code if int
            // mark return for the current call context
            return_flag = 1;
            return_value_holder = return_value;
            return return_value;
        }
        case NODE_PRINT_STATEMENT: {
            struct PrintStatementNode* print_stmt = (struct PrintStatementNode*)node;
            Value val = interpret_expression(print_stmt->expression, env);
            if (val.type == VALUE_INT) {
                printf("%d\n", val.data.int_val);
            } else if (val.type == VALUE_STRING) {
                printf("%s\n", val.data.string_val);
            } else if (val.type == VALUE_FLOAT) {
                printf("%.6f\n", val.data.float_val);
            } else if (val.type == VALUE_BOOL) {
                printf("%s\n", val.data.bool_val ? "true" : "false");
            } else if (val.type == VALUE_NULL) {
                printf("null\n");
            } else if (val.type == VALUE_POINTER) {
                printf("pointer\n");
            }
            value_release(&val);
            return create_value_int(0);
        }
        default:
            // In a real implementation, you might want to call other interpreter functions
            // for expressions, declarations, etc., or handle errors.
            return create_value_int(0);
    }
}