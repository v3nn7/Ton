#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include "token.h"
#include <string.h> // Dodaj to
#include <stdbool.h> // Dodaj to

// Function to free AST nodes recursively
void free_type_node(TypeNode* type_node) {
    if (!type_node) return;
    if (type_node->type_name) {
        free(type_node->type_name);
    }
    if (type_node->base_type) {
        free_type_node(type_node->base_type);
    }
    free(type_node);
}

void free_ast_node(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case NODE_PROGRAM: {
            ProgramNode* program = (ProgramNode*)node;
            for (int i = 0; i < program->num_statements; i++) {
                free_ast_node(program->statements[i]);
            }
            free(program->statements);
            // free(program); // Removed: ASTNode is freed at the end of the function
            break;
        }
        case NODE_VAR_DECLARATION:
        {
            VariableDeclarationNode* var_decl = (VariableDeclarationNode*)node;
            free(var_decl->identifier); // Free the duplicated string
            if (var_decl->initializer) {
                free_ast_node(var_decl->initializer);
            }
            break;
        }
        case NODE_FN_DECLARATION: {
            FunctionDeclarationNode* fn_decl = (FunctionDeclarationNode*)node;
            free_token(fn_decl->identifier);
            for (int i = 0; i < fn_decl->num_parameters; i++) {
                free_ast_node((ASTNode*)fn_decl->parameters[i]);
            }
            free(fn_decl->parameters);
            // No need to free fn_decl->return_type as it's an enum now
            free_ast_node((ASTNode*)fn_decl->body);
            // free(fn_decl); // This line is incorrect, fn_decl is part of the ASTNode
            break;
        }
        case NODE_STRUCT_DECLARATION: {
            struct StructDeclarationNode* struct_decl = (struct StructDeclarationNode*)node;
            if (struct_decl->name) {
                free(struct_decl->name);
            }
            if (struct_decl->fields) {
                for (int i = 0; i < struct_decl->num_fields; i++) {
                    if (struct_decl->fields[i].name) {
                        free((void*)struct_decl->fields[i].name);
                    }
                    if (struct_decl->fields[i].type_name) {
                        free((void*)struct_decl->fields[i].type_name);
                    }
                }
                free(struct_decl->fields);
            }
            // Free methods
            if (struct_decl->methods) {
                for (int i = 0; i < struct_decl->num_methods; i++) {
                    if (struct_decl->methods[i].function) {
                        free_ast_node((ASTNode*)struct_decl->methods[i].function);
                    }
                    if (struct_decl->methods[i].name) {
                        free((void*)struct_decl->methods[i].name);
                    }
                }
                free(struct_decl->methods);
            }
            break;
        }
        case NODE_BLOCK_STATEMENT: {
            BlockStatementNode* block = (BlockStatementNode*)node;
            for (int i = 0; i < block->num_statements; i++) {
                free_ast_node(block->statements[i]);
            }
            free(block->statements);
            // free(block); // Removed: ASTNode is freed at the end of the function
            break;
        }
        case NODE_IF_STATEMENT: {
            IfStatementNode* if_stmt = (IfStatementNode*)node;
            free_ast_node(if_stmt->condition);
            free_ast_node((ASTNode*)if_stmt->consequence);
            free_ast_node((ASTNode*)if_stmt->alternative);
            break;
        }
        case NODE_LOOP_STATEMENT: {
            LoopStatementNode* loop_stmt = (LoopStatementNode*)node;
            if (loop_stmt->iterator) {
                free_token(loop_stmt->iterator);
                free(loop_stmt->iterator);
            }
            free_ast_node(loop_stmt->start_expr);
            free_ast_node(loop_stmt->end_expr);
            free_ast_node(loop_stmt->collection_expr);
            free_ast_node((ASTNode*)loop_stmt->body);
            break;
        }
        case NODE_FOR_STATEMENT: {
            ForStatementNode* for_stmt = (ForStatementNode*)node;
            if (for_stmt->init) free_ast_node(for_stmt->init);
            if (for_stmt->condition) free_ast_node(for_stmt->condition);
            if (for_stmt->update) free_ast_node(for_stmt->update);
            if (for_stmt->body) free_ast_node((ASTNode*)for_stmt->body);
            break;
        }
        case NODE_ARRAY_LITERAL_EXPRESSION: {
            ArrayLiteralExpressionNode* array_lit = (ArrayLiteralExpressionNode*)node;
            for (int i = 0; i < array_lit->num_elements; i++) {
                if (array_lit->elements[i]) free_ast_node(array_lit->elements[i]);
            }
            if (array_lit->elements) free(array_lit->elements);
            break;
        }
        case NODE_ARRAY_ACCESS_EXPRESSION: {
            ArrayAccessExpressionNode* array_access = (ArrayAccessExpressionNode*)node;
            if (array_access->array) free_ast_node(array_access->array);
            if (array_access->index) free_ast_node(array_access->index);
            break;
        }
        case NODE_WHILE_STATEMENT: {
            WhileStatementNode* while_stmt = (WhileStatementNode*)node;
            free_ast_node(while_stmt->condition);
            free_ast_node((ASTNode*)while_stmt->body);
            break;
        }
        case NODE_RETURN_STATEMENT: {
            ReturnStatementNode* return_stmt = (ReturnStatementNode*)node;
            free_ast_node(return_stmt->expression);
            break;
        }
        case NODE_EXPRESSION_STATEMENT: {
            ExpressionStatementNode* expr_stmt = (ExpressionStatementNode*)node;
            free_ast_node(expr_stmt->expression);
            break;
        }
        case NODE_BINARY_EXPRESSION: {
            BinaryExpressionNode* bin_expr = (BinaryExpressionNode*)node;
            free_ast_node(bin_expr->left);
            free_token(bin_expr->operator);
            // free(bin_expr->operator); // Removed: free_token handles this
            free_ast_node(bin_expr->right);
            break;
        }
        case NODE_UNARY_EXPRESSION: {
            UnaryExpressionNode* un_expr = (UnaryExpressionNode*)node;
            free_token(un_expr->operator);
            // free(un_expr->operator); // Removed: free_token handles this
            free_ast_node(un_expr->operand);
            break;
        }
        case NODE_CONDITIONAL_EXPRESSION: {
            ConditionalExpressionNode* cond_expr = (ConditionalExpressionNode*)node;
            free_ast_node(cond_expr->condition);
            free_ast_node(cond_expr->true_expr);
            free_ast_node(cond_expr->false_expr);
            break;
        }
        case NODE_TYPEOF_EXPRESSION: {
            TypeofExpressionNode* typeof_expr = (TypeofExpressionNode*)node;
            free_ast_node(typeof_expr->operand);
            break;
        }
        case NODE_SIZEOF_EXPRESSION: {
            SizeofExpressionNode* sizeof_expr = (SizeofExpressionNode*)node;
            free_ast_node(sizeof_expr->operand);
            break;
        }
        case NODE_ALIGNOF_EXPRESSION: {
            AlignofExpressionNode* alignof_expr = (AlignofExpressionNode*)node;
            free_ast_node(alignof_expr->operand);
            break;
        }
        case NODE_LITERAL_EXPRESSION: {
            LiteralExpressionNode* lit_expr = (LiteralExpressionNode*)node;
            free_token(lit_expr->value);
            // free(lit_expr->value); // Removed: free_token handles this
            break;
        }
        case NODE_IDENTIFIER_EXPRESSION: {
            IdentifierExpressionNode* id_expr = (IdentifierExpressionNode*)node;
            free(id_expr->identifier); // Free the duplicated string
            break;
        }
        case NODE_FN_CALL_EXPRESSION: {
            FunctionCallExpressionNode* fn_call = (FunctionCallExpressionNode*)node;
            free_ast_node(fn_call->callee);
            for (int i = 0; i < fn_call->num_arguments; i++) {
                free_ast_node(fn_call->arguments[i]);
            }
            free(fn_call->arguments);
            break;
        }
        case NODE_PARAMETER: {
            ParameterNode* param = (ParameterNode*)node;
            free_token(param->identifier);
            // No need to free param->param_type as it's an enum now
            // free(param); // Removed: ASTNode is freed at the end of the function
            break;
        }
        case NODE_SWITCH_STATEMENT: {
            SwitchStatementNode* switch_stmt = (SwitchStatementNode*)node;
            free_ast_node(switch_stmt->expression);
            for (int i = 0; i < switch_stmt->num_cases; i++) {
                free_ast_node((ASTNode*)switch_stmt->cases[i]);
            }
            free(switch_stmt->cases);
            break;
        }
        case NODE_CASE_STATEMENT: {
            CaseStatementNode* case_stmt = (CaseStatementNode*)node;
            free_ast_node(case_stmt->value);
            for (int i = 0; i < case_stmt->num_statements; i++) {
                free_ast_node(case_stmt->statements[i]);
            }
            free(case_stmt->statements);
            break;
        }
        case NODE_BREAK_STATEMENT: {
            // BreakStatementNode has no additional fields to free
            break;
        }
        case NODE_CONTINUE_STATEMENT: {
            // ContinueStatementNode has no additional fields to free
            break;
        }
        case NODE_PRINT_STATEMENT: {
            PrintStatementNode* print_stmt = (PrintStatementNode*)node;
            for (int i = 0; i < print_stmt->num_expressions; i++) {
                free_ast_node(print_stmt->expressions[i]);
            }
            if (print_stmt->expressions) ton_free(print_stmt->expressions);
            break;
        }
        case NODE_NEW_EXPRESSION: {
            NewExpressionNode* new_expr = (NewExpressionNode*)node;
            free(new_expr->class_name);
            for (int i = 0; i < new_expr->num_arguments; i++) {
                free_ast_node(new_expr->arguments[i]);
            }
            free(new_expr->arguments);
            break;
        }
        case NODE_IMPORT_STATEMENT: {
            // TODO: Implement freeing of import statement nodes
            break;
        }
        case NODE_CLASS_DECLARATION: {
            // TODO: Implement freeing of class declaration nodes
            break;
        }
        case NODE_MEMBER_ACCESS_EXPRESSION: {
            // TODO: Implement freeing of member access expression nodes
            break;
        }
        case NODE_SUPER_EXPRESSION: {
            // TODO: Implement freeing of super expression nodes
            break;
        }
        case NODE_EXPORT_STATEMENT: {
            // TODO: Implement freeing of export statement nodes
            break;
        }
        case NODE_TRY_STATEMENT: {
            TryStatementNode* try_stmt = (TryStatementNode*)node;
            if (try_stmt->try_block) {
                free_ast_node((ASTNode*)try_stmt->try_block);
            }
            for (int i = 0; i < try_stmt->num_catch_blocks; i++) {
                free_ast_node((ASTNode*)try_stmt->catch_blocks[i]);
            }
            free(try_stmt->catch_blocks);
            if (try_stmt->finally_block) {
                free_ast_node((ASTNode*)try_stmt->finally_block);
            }
            break;
        }
        case NODE_CATCH_STATEMENT: {
            CatchStatementNode* catch_stmt = (CatchStatementNode*)node;
            if (catch_stmt->exception_type) {
                free(catch_stmt->exception_type);
            }
            if (catch_stmt->exception_var) {
                free(catch_stmt->exception_var);
            }
            if (catch_stmt->catch_block) {
                free_ast_node((ASTNode*)catch_stmt->catch_block);
            }
            break;
        }
        case NODE_FINALLY_STATEMENT: {
            FinallyStatementNode* finally_stmt = (FinallyStatementNode*)node;
            if (finally_stmt->finally_block) {
                free_ast_node((ASTNode*)finally_stmt->finally_block);
            }
            break;
        }
        case NODE_THROW_STATEMENT: {
            ThrowStatementNode* throw_stmt = (ThrowStatementNode*)node;
            if (throw_stmt->exception_expr) {
                free_ast_node(throw_stmt->exception_expr);
            }
            break;
        }
        case NODE_MODULE_DECLARATION: {
            // TODO: Implement freeing of module declaration nodes
            break;
        }
        // Add more cases for other node types as they are implemented
    }

    free(node);
}

#include "error.h"

ASTNode* create_binary_expression_node(ASTNode* left, TokenType operator_type, ASTNode* right) {
    BinaryExpressionNode* bin_expr = (BinaryExpressionNode*)malloc(sizeof(BinaryExpressionNode));
    if (bin_expr == NULL) {
        runtime_error("Memory allocation failed for BinaryExpressionNode.");
        return NULL;
    }
    bin_expr->type = NODE_BINARY_EXPRESSION;
    bin_expr->line = left->line; // Assuming left operand has the correct line/column
    bin_expr->column = left->column;
    bin_expr->left = left;
    bin_expr->operator = create_token(operator_type, "", left->line, left->column); // Create a token for the operator
    bin_expr->right = right;
    return (ASTNode*)bin_expr;
}

IdentifierExpressionNode* create_identifier_expression_node(const char* identifier, int line, int column) {
    IdentifierExpressionNode* node = (IdentifierExpressionNode*)malloc(sizeof(IdentifierExpressionNode));
    if (!node) {
        perror("Failed to allocate IdentifierExpressionNode");
        exit(EXIT_FAILURE);
    }
    node->base.type = NODE_IDENTIFIER_EXPRESSION;
    node->base.line = line;
    node->base.column = column;
    node->identifier = strdup(identifier);
    if (!node->identifier) {
        perror("Failed to duplicate identifier string");
        exit(EXIT_FAILURE);
    }
    return node;
}



ASTNode* create_integer_literal_node(int value, int line, int column) {
    LiteralExpressionNode* node = (LiteralExpressionNode*)malloc(sizeof(LiteralExpressionNode));
    if (!node) {
        perror("Failed to allocate LiteralExpressionNode");
        exit(EXIT_FAILURE);
    }
    node->base.type = NODE_LITERAL_EXPRESSION;
    node->base.line = line;
    node->base.column = column;
    // Create a dummy token for the integer literal value
    char* lexeme = (char*)malloc(sizeof(char) * 12); // Max 11 digits + null terminator for int
    sprintf(lexeme, "%d", value);
    node->value = create_token(TOKEN_INT_LITERAL, lexeme, line, column);
    return (ASTNode*)node;
}

ASTNode* create_float_literal_node(double value, int line, int column) {
    LiteralExpressionNode* node = (LiteralExpressionNode*)malloc(sizeof(LiteralExpressionNode));
    if (!node) {
        perror("Failed to allocate LiteralExpressionNode");
        exit(EXIT_FAILURE);
    }
    node->base.type = NODE_LITERAL_EXPRESSION;
    node->base.line = line;
    node->base.column = column;
    // Create a dummy token for the float literal value
    char* lexeme = (char*)malloc(sizeof(char) * 32); // Sufficient for most float representations
    sprintf(lexeme, "%f", value);
    node->value = create_token(TOKEN_FLOAT_LITERAL, lexeme, line, column);
    return (ASTNode*)node;
}

ASTNode* create_string_literal_node(const char* value, int line, int column) {
    LiteralExpressionNode* node = (LiteralExpressionNode*)malloc(sizeof(LiteralExpressionNode));
    if (!node) {
        perror("Failed to allocate LiteralExpressionNode");
        exit(EXIT_FAILURE);
    }
    node->base.type = NODE_LITERAL_EXPRESSION;
    node->base.line = line;
    node->base.column = column;
    node->value = create_token(TOKEN_STRING_LITERAL, strdup(value), line, column);
    return (ASTNode*)node;
}

ASTNode* create_char_literal_node(char value, int line, int column) {
    LiteralExpressionNode* node = (LiteralExpressionNode*)malloc(sizeof(LiteralExpressionNode));
    if (!node) {
        perror("Failed to allocate LiteralExpressionNode");
        exit(EXIT_FAILURE);
    }
    node->base.type = NODE_LITERAL_EXPRESSION;
    node->base.line = line;
    node->base.column = column;
    char* lexeme = (char*)malloc(sizeof(char) * 2); // For char + null terminator
    lexeme[0] = value;
    lexeme[1] = '\0';
    node->value = create_token(TOKEN_CHAR_LITERAL, lexeme, line, column);
    return (ASTNode*)node;
}

ASTNode* create_boolean_literal_node(bool value, int line, int column) {
    LiteralExpressionNode* node = (LiteralExpressionNode*)malloc(sizeof(LiteralExpressionNode));
    if (!node) {
        perror("Failed to allocate LiteralExpressionNode");
        exit(EXIT_FAILURE);
    }
    node->base.type = NODE_LITERAL_EXPRESSION;
    node->base.line = line;
    node->base.column = column;
    node->value = create_token(value ? TOKEN_TRUE : TOKEN_FALSE, strdup(value ? "true" : "false"), line, column);
    return (ASTNode*)node;
}

ASTNode* create_literal_expression_node_null(int line, int column) {
    LiteralExpressionNode* node = (LiteralExpressionNode*)malloc(sizeof(LiteralExpressionNode));
    if (!node) {
        perror("Failed to allocate LiteralExpressionNode for null");
        exit(EXIT_FAILURE);
    }
    node->base.type = NODE_LITERAL_EXPRESSION;
    node->base.line = line;
    node->base.column = column;
    node->value = create_token(TOKEN_NULL, strdup("null"), line, column);
    return (ASTNode*)node;
}

ASTNode* create_new_expression_node(const char* class_name, ASTNode** arguments, int num_arguments, int line, int column) {
    NewExpressionNode* node = (NewExpressionNode*)malloc(sizeof(NewExpressionNode));
    if (!node) {
        perror("Failed to allocate NewExpressionNode");
        exit(EXIT_FAILURE);
    }
    node->base.type = NODE_NEW_EXPRESSION;
    node->base.line = line;
    node->base.column = column;
    node->class_name = strdup(class_name);
    if (!node->class_name) {
        perror("Failed to duplicate class name string");
        exit(EXIT_FAILURE);
    }
    
    // Copy arguments array
    if (num_arguments > 0) {
        node->arguments = malloc(sizeof(ASTNode*) * num_arguments);
        if (!node->arguments) {
            perror("Failed to allocate arguments array");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < num_arguments; i++) {
            node->arguments[i] = arguments[i];
        }
    } else {
        node->arguments = NULL;
    }
    node->num_arguments = num_arguments;
    
    return (ASTNode*)node;
}

ASTNode* create_switch_statement_node(ASTNode* expression, CaseStatementNode** cases, int num_cases, int line, int column) {
    SwitchStatementNode* node = (SwitchStatementNode*)malloc(sizeof(SwitchStatementNode));
    if (!node) {
        perror("Failed to allocate SwitchStatementNode");
        exit(EXIT_FAILURE);
    }
    node->type = NODE_SWITCH_STATEMENT;
    node->line = line;
    node->column = column;
    node->expression = expression;
    node->cases = cases;
    node->num_cases = num_cases;
    return (ASTNode*)node;
}

ASTNode* create_case_statement_node(ASTNode* value, ASTNode** statements, int num_statements, bool is_default, int line, int column) {
    CaseStatementNode* node = (CaseStatementNode*)malloc(sizeof(CaseStatementNode));
    if (!node) {
        perror("Failed to allocate CaseStatementNode");
        exit(EXIT_FAILURE);
    }
    node->type = NODE_CASE_STATEMENT;
    node->line = line;
    node->column = column;
    node->value = value;
    node->statements = statements;
    node->num_statements = num_statements;
    node->is_default = is_default;
    return (ASTNode*)node;
}

ASTNode* create_ast_node(ASTNodeType type, size_t size, int line, int column) {
    ASTNode* node = (ASTNode*)malloc(size);
    if (!node) {
        perror("Failed to allocate ASTNode");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->line = line;
    node->column = column;
    return node;
}