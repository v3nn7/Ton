#include "interpreter_macro.h"
#include "ast.h"
#include "environment.h"
#include "memory.h"
#include "value.h"
#include "interpreter_expr.h"
#include "interpreter_stmt.h"
#include "error.h"
#include <string.h>
#include <stdio.h>

// Function to expand a macro call by substituting parameters
ASTNode* expand_macro_call(MacroCallExpressionNode* macro_call, Environment* env) {
    // Look up the macro definition in the environment
    Value* macro_value = env_get_variable(env, macro_call->macro_name);
    
    if (!macro_value || macro_value->type != VALUE_MACRO) {
        // Macro not found or not a macro value
        printf("Error: Macro '%s' not found\n", macro_call->macro_name);
        return NULL;
    }
    
    MacroDeclarationNode* macro_def = (MacroDeclarationNode*)macro_value->data.macro_def;
    
    // Create a copy of the macro body for expansion
    BlockStatementNode* expanded_body = ton_malloc(sizeof(BlockStatementNode));
    expanded_body->type = NODE_BLOCK_STATEMENT;
    expanded_body->line = macro_call->base.line;
    expanded_body->column = macro_call->base.column;
    
    // Copy statements from original body
    expanded_body->statements = ton_malloc(sizeof(ASTNode*) * macro_def->body->num_statements);
    expanded_body->num_statements = macro_def->body->num_statements;
    
    for (int i = 0; i < macro_def->body->num_statements; i++) {
        // TODO: Implement parameter substitution here
        // This would involve walking the AST and replacing parameter identifiers
        // with the corresponding argument expressions
        expanded_body->statements[i] = macro_def->body->statements[i];
    }
    
    return (ASTNode*)expanded_body;
}

// Function to evaluate a macro call expression
Value evaluate_macro_call_expression(MacroCallExpressionNode* node, Environment* env) {
    // Expand the macro
    ASTNode* expanded_body = expand_macro_call(node, env);
    
    if (!expanded_body) {
        return create_value_null();
    }
    
    // Execute the expanded macro body
    Value result;
    TonError error = interpret_statement(expanded_body, env, &result);
    
    if (error.code != TON_OK && error.code != TON_RETURN) {
        printf("Error executing macro: %s\n", error.message);
        return create_value_null();
    }
    
    return result;
}

// Function to declare a macro in the environment
void declare_macro(MacroDeclarationNode* macro_decl, Environment* env) {
    // Create a macro value
    Value macro_value = create_value_macro((void*)macro_decl);
    
    // Store the macro in the environment
    env_add_variable(env, macro_decl->identifier, macro_value, VAR_TYPE_UNKNOWN);
}