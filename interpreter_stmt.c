#include <stdio.h>
#include "interpreter_stmt.h"
#include "interpreter.h"
#include "environment.h"
#include "ast.h"
#include "value.h"
#include "builtin.h"
#include "memory.h"

#include "interpreter_expr.h"

TonError interpret_statement(ASTNode* node, Environment* env, Value* out_result) {
    if (!node || !env || !out_result) {
        return ton_error(TON_ERR_RUNTIME, "Invalid arguments");
    }
    *out_result = create_value_null();

    switch (node->type) {
        case NODE_PROGRAM: {
            ProgramNode* program = (ProgramNode*)node;
            TonError err = ton_ok();
            for (int i = 0; i < program->num_statements; i++) {
                Value stmt_result;
                err = interpret_statement(program->statements[i], env, &stmt_result);
                value_release(&stmt_result);
                if (err.code != TON_OK) {
                    return err;
                }
            }
            return err;
        }
        case NODE_BLOCK_STATEMENT: {
            BlockStatementNode* block = (BlockStatementNode*)node;
            Environment* block_env = create_child_environment(env);
            TonError err;
            for (int i = 0; i < block->num_statements; i++) {
                err = interpret_statement(block->statements[i], block_env, out_result);
                if (err.code != TON_OK) {
                    destroy_environment(block_env);
                    return err;
                }
            }
            destroy_environment(block_env);
            return ton_ok();
        }
        case NODE_IF_STATEMENT: {
            IfStatementNode* if_stmt = (IfStatementNode*)node;
            Value cond;
            TonError err = interpret_expression(if_stmt->condition, env, &cond);
            if (err.code != TON_OK) return err;

            bool condition_result = false;
            if (cond.type == VALUE_BOOL) {
                condition_result = cond.data.bool_val;
            } else if (cond.type == VALUE_INT) {
                condition_result = cond.data.int_val != 0;
            } else {
                value_release(&cond);
                return ton_error(TON_ERR_TYPE, "If condition must be bool or int");
            }
            value_release(&cond);

            if (condition_result) {
                return interpret_statement((ASTNode*)if_stmt->consequence, env, out_result);
            } else if (if_stmt->alternative) {
                return interpret_statement((ASTNode*)if_stmt->alternative, env, out_result);
            }
            return ton_ok();
        }
        case NODE_WHILE_STATEMENT: {
            WhileStatementNode* wh = (WhileStatementNode*)node;
            TonError err;
            while (1) {
                Value cond;
                err = interpret_expression(wh->condition, env, &cond);
                if (err.code != TON_OK) return err;

                bool condition_result = false;
                if (cond.type == VALUE_BOOL) condition_result = cond.data.bool_val;
                else if (cond.type == VALUE_INT) condition_result = cond.data.int_val != 0;
                else {
                    value_release(&cond);
                    return ton_error(TON_ERR_TYPE, "While condition must be bool or int");
                }
                value_release(&cond);

                if (!condition_result) break;

                err = interpret_statement((ASTNode*)wh->body, env, out_result);
                if (err.code == TON_RETURN) return err;
                if (err.code == TON_BREAK) break;
                if (err.code == TON_CONTINUE) continue;
                if (err.code != TON_OK) return err;
            }
            return ton_ok();
        }
        case NODE_FOR_STATEMENT: {
            ForStatementNode* for_stmt = (ForStatementNode*)node;
            Environment* loop_env = create_child_environment(env);
            TonError err;
            if (for_stmt->init) {
                err = interpret_expression(for_stmt->init, loop_env, out_result);
                if (err.code != TON_OK) {
                    destroy_environment(loop_env);
                    return err;
                }
            }

            while (1) {
                bool should_continue = true;
                if (for_stmt->condition) {
                    Value cond;
                    err = interpret_expression(for_stmt->condition, loop_env, &cond);
                    if (err.code != TON_OK) {
                        destroy_environment(loop_env);
                        return err;
                    }
                    should_continue = (cond.type == VALUE_INT && cond.data.int_val != 0) ||
                                      (cond.type == VALUE_BOOL && cond.data.bool_val);
                    value_release(&cond);
                    if (!should_continue) break;
                }

                err = interpret_statement((ASTNode*)for_stmt->body, loop_env, out_result);
                if (err.code == TON_RETURN) {
                    destroy_environment(loop_env);
                    return err;
                }
                if (err.code == TON_BREAK) break;
                if (err.code == TON_CONTINUE) {
                    // Continue to update
                } else if (err.code != TON_OK) {
                    destroy_environment(loop_env);
                    return err;
                }

                if (for_stmt->update) {
                    err = interpret_expression(for_stmt->update, loop_env, out_result);
                    if (err.code != TON_OK) {
                        destroy_environment(loop_env);
                        return err;
                    }
                }
            }
            destroy_environment(loop_env);
            return ton_ok();
        }
        case NODE_LOOP_STATEMENT: {
            LoopStatementNode* loop = (LoopStatementNode*)node;
            TonError err;
            while (1) {
                err = interpret_statement((ASTNode*)loop->body, env, out_result);
                if (err.code == TON_RETURN) return err;
                if (err.code == TON_BREAK) break;
                if (err.code == TON_CONTINUE) continue;
                if (err.code != TON_OK) return err;
            }
            return ton_ok();
        }
        case NODE_SWITCH_STATEMENT: {
            SwitchStatementNode* switch_stmt = (SwitchStatementNode*)node;
            Value switch_val;
            TonError err = interpret_expression(switch_stmt->expression, env, &switch_val);
            if (err.code != TON_OK) return err;
            if (switch_val.type != VALUE_INT) {
                value_release(&switch_val);
                return ton_error(TON_ERR_TYPE, "Switch expression must be int");
            }
            int switch_int = switch_val.data.int_val;
            value_release(&switch_val);

            bool matched = false;
            for (int i = 0; i < switch_stmt->num_cases; i++) {
                CaseStatementNode* case_stmt = switch_stmt->cases[i];
                Value case_val;
                err = interpret_expression(case_stmt->value, env, &case_val);
                if (err.code != TON_OK) return err;
                if (case_val.type != VALUE_INT) {
                    value_release(&case_val);
                    return ton_error(TON_ERR_TYPE, "Case value must be int");
                }
                if (case_val.data.int_val == switch_int) {
                    matched = true;
                    value_release(&case_val);
                    for (int j = 0; j < case_stmt->num_statements; j++) {
                        err = interpret_statement(case_stmt->statements[j], env, out_result);
                        if (err.code != TON_OK) return err;
                    }
                    break; // Assuming no fallthrough
                }
                value_release(&case_val);
            }

            if (!matched && switch_stmt->default_case) {
                return interpret_statement((ASTNode*)switch_stmt->default_case, env, out_result);
            }
            return ton_ok();
        }
        case NODE_BREAK_STATEMENT:
            return ton_error(TON_BREAK, "Break");
        case NODE_CONTINUE_STATEMENT:
            return ton_error(TON_CONTINUE, "Continue");
        case NODE_EXPRESSION_STATEMENT:
            ExpressionStatementNode* expr_stmt = (ExpressionStatementNode*)node;
            return interpret_expression(expr_stmt->expression, env, out_result);
        case NODE_RETURN_STATEMENT: {
            ReturnStatementNode* ret = (ReturnStatementNode*)node;
            TonError err = interpret_expression(ret->expression, env, out_result);
            if (err.code != TON_OK) return err;
            return ton_error(TON_RETURN, "Return");
        }
        case NODE_PRINT_STATEMENT: {
            PrintStatementNode* print = (PrintStatementNode*)node;
            Value val;
            TonError err = interpret_expression(print->expression, env, &val);
            if (err.code != TON_OK) return err;

            switch (val.type) {
                case VALUE_INT: printf("%d\n", val.data.int_val); break;
                case VALUE_STRING: printf("%s\n", val.data.string_val); break;
                case VALUE_FLOAT: printf("%.6f\n", val.data.float_val); break;
                case VALUE_BOOL: printf("%s\n", val.data.bool_val ? "true" : "false"); break;
                case VALUE_NULL: printf("null\n"); break;
                case VALUE_POINTER: printf("pointer\n"); break;
                default: printf("<unknown>\n");
            }
            value_release(&val);
            return ton_ok();
        }
        case NODE_FN_DECLARATION: {
            FunctionDeclarationNode* fn_decl = (FunctionDeclarationNode*)node;
            Function* func = ton_malloc(sizeof(Function));
            if (!func) {
                return ton_error(TON_ERR_MEMORY, "Failed to allocate Function");
            }
            func->name = ton_strdup(fn_decl->identifier->lexeme);
            func->body = (ASTNode*)fn_decl->body;
            func->closure_env = env; // Capture current environment for closures
            func->parameters = fn_decl->parameters; // Assuming AST ownership
            func->num_parameters = fn_decl->num_parameters;
            func->return_type = fn_decl->return_type;

            Value fn_val = create_value_fn(func);
            env_add_variable(env, func->name, fn_val);

            return ton_ok();
        }
        default:
            return ton_error(TON_ERR_RUNTIME, "Unsupported statement type");
    }
}