#include "parser.h" 
 #include <stdlib.h> 
 #include <stdio.h> 
 #include <string.h> 
 #include <stdbool.h> 
 #include "ast.h"
 #include "lexer.h"
 #include "struct.h"  // For access modifiers
 #include "memory.h"

// Safe local strdup replacement for parser identifiers
static char* my_strdup_parser(const char* s) {
    if (s == NULL) return NULL;
    size_t len = strlen(s) + 1;
    char* d = (char*)ton_malloc(len);
    if (!d) return NULL;
    memcpy(d, s, len);
    return d;
}

 void init_parser(Parser* parser, Lexer* lexer) { 
     parser->lexer = lexer; 
     parser->current_token = get_next_token(lexer); 
     parser->peek_token = get_next_token(lexer); 
 } 
  
 static void next_token(Parser* parser) { 
     free_token(parser->current_token); 
     parser->current_token = parser->peek_token; 
     parser->peek_token = get_next_token(parser->lexer); 
     #ifdef TON_DEBUG
     printf("DEBUG: next_token: %s (lexeme: %s)\n",
            token_type_to_string(parser->current_token->type),
            parser->current_token->lexeme);
     #endif
 } 
  
 static int match_token(Parser* parser, TokenType type) { 
     return parser->current_token->type == type; 
 } 
  
 static void expect_token(Parser* parser, TokenType type, const char* msg) { 
     if (!match_token(parser, type)) { 
         parser_error(parser, msg); 
     } 
 } 
  
 #include "error.h"

void parser_error(Parser* parser, const char* msg) { 
     runtime_error("Parser Error at line %d, column %d: %s",
                    parser->current_token->line,
                    parser->current_token->column,
                    msg); 
 } 
  
 // Forward declarations 
 ASTNode* parse_statement(Parser* parser); 
 ASTNode* parse_expression(Parser* parser, int min_precedence); 
 ASTNode* parse_block_statement(Parser* parser); 
 ASTNode* parse_function_call_expression(Parser* parser, ASTNode* callee); 
 TypeNode* parse_type(Parser* parser); 
  
 int get_precedence(TokenType type) { 
    switch (type) { 
        case TOKEN_ASSIGN: 
        case TOKEN_PLUS_ASSIGN: 
        case TOKEN_MINUS_ASSIGN: 
        case TOKEN_STAR_ASSIGN: 
        case TOKEN_SLASH_ASSIGN: 
        case TOKEN_MODULO_ASSIGN: return 1; 
        case TOKEN_QUESTION: return 2; // Ternary operator has low precedence
        case TOKEN_EQ: case TOKEN_NEQ: return 3; 
        case TOKEN_LT: case TOKEN_LE: case TOKEN_GT: case TOKEN_GE: return 4; 
        case TOKEN_PLUS: case TOKEN_MINUS: return 5; 
        case TOKEN_STAR: case TOKEN_SLASH: return 6; 
        default: return 0; 
    } 
} 
  
 ASTNode* parse_program(Parser* parser) {
    ProgramNode* program = ton_malloc(sizeof(ProgramNode)); 
     program->type = NODE_PROGRAM; 
     program->statements = NULL; 
     program->num_statements = 0; 
  
     int capacity = 10;
    ASTNode** stmts = ton_malloc(sizeof(ASTNode*) * capacity); 
     int count = 0; 
  
     while (!match_token(parser, TOKEN_EOF)) { 
         ASTNode* stmt = parse_statement(parser); 
         if (stmt) { 
             if (count >= capacity) { 
                 capacity *= 2; 
                 stmts = ton_realloc(stmts, sizeof(ASTNode*) * capacity); 
             } 
             stmts[count++] = stmt; 
         } 
     } 
  
     program->statements = stmts; 
     program->num_statements = count; 
     return (ASTNode*)program; 
 } 
  
 // ---------- STATEMENTS ---------- 
  
 ASTNode* parse_statement(Parser* parser) { 
    switch (parser->current_token->type) { 
        case TOKEN_LET: 
        case TOKEN_CONST: 
        case TOKEN_VAR: 
            return parse_variable_declaration(parser); 
        case TOKEN_FN:
           return parse_function_declaration(parser);
       case TOKEN_STRUCT:
           return parse_struct_declaration(parser);
       case TOKEN_CLASS:
           return parse_class_declaration(parser);
       case TOKEN_IMPORT:
           return parse_import_statement(parser);
       case TOKEN_RETURN: 
            return parse_return_statement(parser); 
        case TOKEN_PRINT: 
            return parse_print_statement(parser); 
        case TOKEN_IF: 
            return parse_if_statement(parser); 
        case TOKEN_LOOP: 
           return parse_loop_statement(parser); 
       case TOKEN_FOR:
           return parse_for_statement(parser);
       case TOKEN_WHILE:
           return parse_while_statement(parser);
       case TOKEN_SWITCH:
           return parse_switch_statement(parser);
       case TOKEN_BREAK:
           return parse_break_statement(parser);
         case TOKEN_CONTINUE:
             return parse_continue_statement(parser);
        case TOKEN_LBRACE:
             return parse_block_statement(parser);
         case TOKEN_SEMICOLON: // allow empty statements
             next_token(parser);
             return NULL; // Empty statement
        default: 
            return parse_expression_statement(parser); 
    } 
} 
 
 ASTNode* parse_variable_declaration(Parser* parser) {
    VariableDeclarationNode* var_decl = ton_malloc(sizeof(VariableDeclarationNode)); 
    var_decl->base.type = NODE_VAR_DECLARATION; 
    var_decl->base.line = parser->current_token->line; 
    var_decl->base.column = parser->current_token->column; 

    var_decl->is_const = (parser->current_token->type == TOKEN_CONST); 
    next_token(parser); // consume let/const/var 

    expect_token(parser, TOKEN_IDENTIFIER, "Expected identifier"); 
    var_decl->identifier = my_strdup_parser(parser->current_token->lexeme); 
    next_token(parser); 

    if (match_token(parser, TOKEN_COLON)) { 
        next_token(parser); 
        TypeNode* type_node = parse_type(parser); 
        if (!type_node) parser_error(parser, "Expected type"); 
        var_decl->var_type = type_node->var_type; 
        var_decl->is_array = (type_node->var_type == VAR_TYPE_ARRAY); 
        free(type_node); 
    } else { 
        var_decl->var_type = VAR_TYPE_INFERRED; 
        var_decl->is_array = 0; 
    } 

    var_decl->array_size = 0; // Dynamic size for now 

    var_decl->initializer = NULL; 
    if (match_token(parser, TOKEN_ASSIGN)) { 
        next_token(parser); 
        var_decl->initializer = parse_expression(parser, 0); 
    } 

    expect_token(parser, TOKEN_SEMICOLON, "Expected ';' after variable declaration"); 
    next_token(parser); 

    return (ASTNode*)var_decl; 
} 
 
 ASTNode* parse_function_declaration(Parser* parser) {
    FunctionDeclarationNode* func_decl = ton_malloc(sizeof(FunctionDeclarationNode)); 
     func_decl->type = NODE_FN_DECLARATION; 
     func_decl->line = parser->current_token->line; 
     func_decl->column = parser->current_token->column; 
     func_decl->parameters = NULL; 
     func_decl->num_parameters = 0; 
     func_decl->return_type = VAR_TYPE_VOID; 
     func_decl->body = NULL; 
 
     expect_token(parser, TOKEN_FN, "Expected 'fn'"); 
     next_token(parser); 
 
     #ifdef TON_DEBUG
     printf("DEBUG: In parse_function_declaration, after consuming FN. Current token: %s (type: %d, lexeme: %s)\n",
            token_type_to_string(parser->current_token->type),
            parser->current_token->type,
            parser->current_token->lexeme);
     #endif
 
     expect_token(parser, TOKEN_IDENTIFIER, "Expected function name"); 
     func_decl->identifier = create_token(parser->current_token->type, parser->current_token->lexeme, parser->current_token->line, parser->current_token->column);
     next_token(parser); 
 
     expect_token(parser, TOKEN_LPAREN, "Expected '('"); 
     next_token(parser); 
 
     int param_capacity = 4;
    ParameterNode** parameters = ton_malloc(sizeof(ParameterNode*) * param_capacity); 
 
     while (!match_token(parser, TOKEN_RPAREN) && !match_token(parser, TOKEN_EOF)) { 
         ParameterNode* param = ton_malloc(sizeof(ParameterNode));
         param->type = NODE_PARAMETER;
         param->line = parser->current_token->line;
         param->column = parser->current_token->column;
 
         expect_token(parser, TOKEN_IDENTIFIER, "Expected parameter name");
         param->identifier = create_token(parser->current_token->type, parser->current_token->lexeme, parser->current_token->line, parser->current_token->column);
         next_token(parser);
 
         expect_token(parser, TOKEN_COLON, "Expected ':' after parameter name");
         next_token(parser);
 
         TypeNode* type_node = parse_type(parser);
         if (!type_node) parser_error(parser, "Expected parameter type");
         param->param_type = type_node->var_type;
         free(type_node);
 
         if (func_decl->num_parameters >= param_capacity) {
             param_capacity *= 2;
             parameters = realloc(parameters, sizeof(ParameterNode*) * param_capacity);
         }
         parameters[func_decl->num_parameters++] = param;
 
         if (match_token(parser, TOKEN_COMMA)) next_token(parser);
         else if (!match_token(parser, TOKEN_RPAREN)) parser_error(parser, "Expected ',' or ')'");
     }

     func_decl->parameters = parameters;
     expect_token(parser, TOKEN_RPAREN, "Expected ')'");
     next_token(parser);

     // Optional return type
     if (match_token(parser, TOKEN_COLON) || match_token(parser, TOKEN_ARROW)) {
         next_token(parser);
         TypeNode* ret_type = parse_type(parser);
         if (!ret_type) parser_error(parser, "Expected return type");
         func_decl->return_type = ret_type->var_type;
         free(ret_type);
     }

     func_decl->body = (BlockStatementNode*)parse_block_statement(parser);
     return (ASTNode*)func_decl;
 }

 ASTNode* parse_if_statement(Parser* parser) {
     IfStatementNode* if_stmt = ton_malloc(sizeof(IfStatementNode));
     if_stmt->type = NODE_IF_STATEMENT;
     if_stmt->line = parser->current_token->line;
     if_stmt->column = parser->current_token->column;

     expect_token(parser, TOKEN_IF, "Expected 'if'");
     next_token(parser);

     expect_token(parser, TOKEN_LPAREN, "Expected '(' after 'if'");
     next_token(parser);
     if_stmt->condition = parse_expression(parser, 0);
     expect_token(parser, TOKEN_RPAREN, "Expected ')'");
     next_token(parser);

     if_stmt->consequence = (BlockStatementNode*)parse_statement(parser);

     if (match_token(parser, TOKEN_ELSE)) {
         next_token(parser);
         if_stmt->alternative = (BlockStatementNode*)parse_statement(parser);
     } else {
         if_stmt->alternative = NULL;
     }
     return (ASTNode*)if_stmt;
 }

 ASTNode* parse_loop_statement(Parser* parser) {
     LoopStatementNode* loop_stmt = ton_malloc(sizeof(LoopStatementNode));
     loop_stmt->type = NODE_LOOP_STATEMENT;
     loop_stmt->line = parser->current_token->line;
     loop_stmt->column = parser->current_token->column;
     loop_stmt->body = NULL;

     expect_token(parser, TOKEN_LOOP, "Expected 'loop'");
     next_token(parser);

     loop_stmt->body = (BlockStatementNode*)parse_block_statement(parser);
     return (ASTNode*)loop_stmt;
 }

ASTNode* parse_for_statement(Parser* parser) {
    ForStatementNode* for_stmt = (ForStatementNode*)ton_malloc(sizeof(ForStatementNode));
    if (!for_stmt) parser_error(parser, "Out of memory while parsing for-statement");
    for_stmt->type = NODE_FOR_STATEMENT;
    for_stmt->line = parser->current_token->line;
    for_stmt->column = parser->current_token->column;
    for_stmt->init = NULL;
    for_stmt->condition = NULL;
    for_stmt->update = NULL;
    for_stmt->body = NULL;

    expect_token(parser, TOKEN_FOR, "Expected 'for'");
    next_token(parser);

    expect_token(parser, TOKEN_LPAREN, "Expected '(' after 'for'");
    next_token(parser);

    // Parse initialization (can be empty)
    if (parser->current_token->type != TOKEN_SEMICOLON) {
        for_stmt->init = parse_statement(parser);
    } else {
        next_token(parser); // consume semicolon
    }

    // Parse condition (can be empty)
    if (parser->current_token->type != TOKEN_SEMICOLON) {
        for_stmt->condition = parse_expression(parser, 0);
    }
    expect_token(parser, TOKEN_SEMICOLON, "Expected ';' after for condition");
    next_token(parser);

    // Parse update (can be empty)
    if (parser->current_token->type != TOKEN_RPAREN) {
        for_stmt->update = parse_expression(parser, 0);
    }
    expect_token(parser, TOKEN_RPAREN, "Expected ')' after for update");
    next_token(parser);

    // Parse body
    for_stmt->body = (BlockStatementNode*)parse_block_statement(parser);
    return (ASTNode*)for_stmt;
}

ASTNode* parse_while_statement(Parser* parser) {
     WhileStatementNode* while_stmt = (WhileStatementNode*)ton_malloc(sizeof(WhileStatementNode));
     if (!while_stmt) parser_error(parser, "Out of memory while parsing while-statement");
     while_stmt->type = NODE_WHILE_STATEMENT;
     while_stmt->line = parser->current_token->line;
     while_stmt->column = parser->current_token->column;

     expect_token(parser, TOKEN_WHILE, "Expected 'while'");
     next_token(parser);

     expect_token(parser, TOKEN_LPAREN, "Expected '(' after 'while'");
     next_token(parser);
     while_stmt->condition = parse_expression(parser, 0);
     expect_token(parser, TOKEN_RPAREN, "Expected ')' after while condition");
     next_token(parser);

     while_stmt->body = (BlockStatementNode*)parse_block_statement(parser);
     return (ASTNode*)while_stmt;
 }

 ASTNode* parse_switch_statement(Parser* parser) {
     SwitchStatementNode* switch_stmt = ton_malloc(sizeof(SwitchStatementNode));
     switch_stmt->type = NODE_SWITCH_STATEMENT;
     switch_stmt->line = parser->current_token->line;
     switch_stmt->column = parser->current_token->column;
     switch_stmt->cases = NULL;
     switch_stmt->num_cases = 0;
     switch_stmt->default_case = NULL;

     expect_token(parser, TOKEN_SWITCH, "Expected 'switch'");
     next_token(parser);

     expect_token(parser, TOKEN_LPAREN, "Expected '(' after 'switch'");
     next_token(parser);
     switch_stmt->expression = parse_expression(parser, 0);
     expect_token(parser, TOKEN_RPAREN, "Expected ')' after switch expression");
     next_token(parser);

     expect_token(parser, TOKEN_LBRACE, "Expected '{' after switch expression");
     next_token(parser);

     // Parse cases and default
     int case_capacity = 4;
     CaseStatementNode** cases = ton_malloc(sizeof(CaseStatementNode*) * case_capacity);

     while (!match_token(parser, TOKEN_RBRACE) && !match_token(parser, TOKEN_EOF)) {
         if (match_token(parser, TOKEN_CASE)) {
             next_token(parser);
             
             CaseStatementNode* case_stmt = ton_malloc(sizeof(CaseStatementNode));
             case_stmt->type = NODE_CASE_STATEMENT;
             case_stmt->line = parser->current_token->line;
             case_stmt->column = parser->current_token->column;
             case_stmt->value = parse_expression(parser, 0);
             
             expect_token(parser, TOKEN_COLON, "Expected ':' after case value");
             next_token(parser);

             // Parse statements until next case/default/end
             int stmt_capacity = 4;
             ASTNode** statements = ton_malloc(sizeof(ASTNode*) * stmt_capacity);
             int stmt_count = 0;

             while (!match_token(parser, TOKEN_CASE) && !match_token(parser, TOKEN_DEFAULT) && 
                    !match_token(parser, TOKEN_RBRACE) && !match_token(parser, TOKEN_EOF)) {
                 ASTNode* stmt = parse_statement(parser);
                 if (stmt) {
                     if (stmt_count >= stmt_capacity) {
                         stmt_capacity *= 2;
                         statements = realloc(statements, sizeof(ASTNode*) * stmt_capacity);
                     }
                     statements[stmt_count++] = stmt;
                 }
             }

             case_stmt->statements = statements;
             case_stmt->num_statements = stmt_count;

             if (switch_stmt->num_cases >= case_capacity) {
                 case_capacity *= 2;
                 cases = realloc(cases, sizeof(CaseStatementNode*) * case_capacity);
             }
             cases[switch_stmt->num_cases++] = case_stmt;

         } else if (match_token(parser, TOKEN_DEFAULT)) {
             next_token(parser);
             expect_token(parser, TOKEN_COLON, "Expected ':' after 'default'");
             next_token(parser);

             // Parse default case as a block
             switch_stmt->default_case = (BlockStatementNode*)parse_block_statement(parser);

         } else {
             parser_error(parser, "Expected 'case' or 'default' in switch statement");
         }
     }

     switch_stmt->cases = cases;
     expect_token(parser, TOKEN_RBRACE, "Expected '}' to close switch statement");
     next_token(parser);

     return (ASTNode*)switch_stmt;
 }

 ASTNode* parse_break_statement(Parser* parser) {
     BreakStatementNode* break_stmt = ton_malloc(sizeof(BreakStatementNode));
     break_stmt->type = NODE_BREAK_STATEMENT;
     break_stmt->line = parser->current_token->line;
     break_stmt->column = parser->current_token->column;
     next_token(parser); // consume 'break'
     return (ASTNode*)break_stmt;
 }

ASTNode* parse_continue_statement(Parser* parser) {
    ContinueStatementNode* cont_stmt = ton_malloc(sizeof(ContinueStatementNode));
    if (!cont_stmt) parser_error(parser, "Out of memory while parsing continue-statement");
    cont_stmt->type = NODE_CONTINUE_STATEMENT;
    cont_stmt->line = parser->current_token->line;
    cont_stmt->column = parser->current_token->column;
    next_token(parser); // consume 'continue'
    return (ASTNode*)cont_stmt;
}
 ASTNode* parse_return_statement(Parser* parser) {
     ReturnStatementNode* ret_stmt = ton_malloc(sizeof(ReturnStatementNode));
     ret_stmt->type = NODE_RETURN_STATEMENT;
     ret_stmt->line = parser->current_token->line;
     ret_stmt->column = parser->current_token->column;

     expect_token(parser, TOKEN_RETURN, "Expected 'return'");
     next_token(parser);

     if (!match_token(parser, TOKEN_SEMICOLON))
         ret_stmt->expression = parse_expression(parser, 0);
     else
         ret_stmt->expression = NULL;

     expect_token(parser, TOKEN_SEMICOLON, "Expected ';'");
     next_token(parser);

     return (ASTNode*)ret_stmt;
 }

 ASTNode* parse_print_statement(Parser* parser) {
     struct PrintStatementNode* print_stmt = ton_malloc(sizeof(struct PrintStatementNode));
     print_stmt->base.type = NODE_PRINT_STATEMENT;
     print_stmt->base.line = parser->current_token->line;
     print_stmt->base.column = parser->current_token->column;

     expect_token(parser, TOKEN_PRINT, "Expected 'print'");
     next_token(parser);

     expect_token(parser, TOKEN_LPAREN, "Expected '('");
     next_token(parser);

     print_stmt->expression = parse_expression(parser, 0);

     expect_token(parser, TOKEN_RPAREN, "Expected ')'");
     next_token(parser);

     expect_token(parser, TOKEN_SEMICOLON, "Expected ';' after print statement");
     next_token(parser);

     return (ASTNode*)print_stmt;
 }
 ASTNode* parse_block_statement(Parser* parser) {
     expect_token(parser, TOKEN_LBRACE, "Expected '{' to start block");
     next_token(parser);

     BlockStatementNode* block = ton_malloc(sizeof(BlockStatementNode));
     block->type = NODE_BLOCK_STATEMENT;
     block->line = parser->current_token->line;
     block->column = parser->current_token->column;

     int capacity = 10;
     ASTNode** stmts = ton_malloc(sizeof(ASTNode*) * capacity);
     int count = 0;

     while (!match_token(parser, TOKEN_RBRACE) && !match_token(parser, TOKEN_EOF)) {
         ASTNode* stmt = parse_statement(parser);
         if (stmt) {
             if (count >= capacity) {
                 capacity *= 2;
                 stmts = ton_realloc(stmts, sizeof(ASTNode*) * capacity);
             }
             stmts[count++] = stmt;
         }
     }

     expect_token(parser, TOKEN_RBRACE, "Expected '}' to close block");
     next_token(parser);

     block->statements = stmts;
     block->num_statements = count;
     return (ASTNode*)block;
 }

 // ---------- EXPRESSIONS ----------
 ASTNode* parse_expression_statement(Parser* parser) {
    int line = parser->current_token->line;
    int column = parser->current_token->column;
    ASTNode* expr = parse_expression(parser, 0);
    ExpressionStatementNode* expr_stmt = (ExpressionStatementNode*)create_ast_node(NODE_EXPRESSION_STATEMENT, sizeof(ExpressionStatementNode), line, column);
    expr_stmt->expression = expr;
    expect_token(parser, TOKEN_SEMICOLON, "Expected ';' after expression");
    next_token(parser);
    return (ASTNode*)expr_stmt;
}

 ASTNode* parse_expression(Parser* parser, int min_precedence) {
     ASTNode* left = NULL;

     switch (parser->current_token->type) {
         case TOKEN_INT_LITERAL: {
            int value;
            // Check for hexadecimal literals
            if (strncmp(parser->current_token->lexeme, "0x", 2) == 0 || strncmp(parser->current_token->lexeme, "0X", 2) == 0) {
                value = (int)strtol(parser->current_token->lexeme, NULL, 16);
            }
            // Check for binary literals
            else if (strncmp(parser->current_token->lexeme, "0b", 2) == 0 || strncmp(parser->current_token->lexeme, "0B", 2) == 0) {
                value = (int)strtol(parser->current_token->lexeme + 2, NULL, 2);
            }
            // Regular decimal literals
            else {
                value = atoi(parser->current_token->lexeme);
            }
            left = create_integer_literal_node(value, parser->current_token->line, parser->current_token->column);
            next_token(parser);
            break;
        }
         case TOKEN_FLOAT_LITERAL: {
             double value = atof(parser->current_token->lexeme);
             left = create_float_literal_node(value, parser->current_token->line, parser->current_token->column);
             next_token(parser);
             break;
         }
         case TOKEN_STRING_LITERAL: {
             left = create_string_literal_node(parser->current_token->lexeme, parser->current_token->line, parser->current_token->column);
             next_token(parser);
             break;
         }
         case TOKEN_TRUE:
         case TOKEN_FALSE: {
             bool value = (parser->current_token->type == TOKEN_TRUE);
             left = create_boolean_literal_node(value, parser->current_token->line, parser->current_token->column);
             next_token(parser);
             break;
         }
         case TOKEN_IDENTIFIER: {
             left = (ASTNode*)create_identifier_expression_node(parser->current_token->lexeme, parser->current_token->line, parser->current_token->column);
             next_token(parser);

             if (match_token(parser, TOKEN_LPAREN)) {
                 left = parse_function_call_expression(parser, left);
             }
             break;
         }
         case TOKEN_THIS: {
             left = (ASTNode*)create_identifier_expression_node("this", parser->current_token->line, parser->current_token->column);
             next_token(parser);
             break;
         }
         case TOKEN_LPAREN: {
             next_token(parser);
             left = parse_expression(parser, 0);
             expect_token(parser, TOKEN_RPAREN, "Expected ')'");
             next_token(parser);
             break;
         }
        case TOKEN_LBRACKET: {
            // Array literal: [1, 2, 3]
            ArrayLiteralExpressionNode* array_lit = ton_malloc(sizeof(ArrayLiteralExpressionNode));
            array_lit->base.type = NODE_ARRAY_LITERAL_EXPRESSION;
            array_lit->base.line = parser->current_token->line;
            array_lit->base.column = parser->current_token->column;
            array_lit->elements = NULL;
            array_lit->num_elements = 0;
            array_lit->element_type = VAR_TYPE_UNKNOWN;
            
            next_token(parser); // consume '['
            
            if (!match_token(parser, TOKEN_RBRACKET)) {
                // Parse array elements
                int capacity = 4;
                array_lit->elements = ton_malloc(sizeof(ASTNode*) * capacity);
                
                do {
                    if (array_lit->num_elements >= capacity) {
                        capacity *= 2;
                        array_lit->elements = ton_realloc(array_lit->elements, sizeof(ASTNode*) * capacity);
                    }
                    array_lit->elements[array_lit->num_elements++] = parse_expression(parser, 0);
                    
                    if (match_token(parser, TOKEN_COMMA)) {
                        next_token(parser);
                    } else {
                        break;
                    }
                } while (parser->current_token->type != TOKEN_RBRACKET);
            }
            
            expect_token(parser, TOKEN_RBRACKET, "Expected ']' after array elements");
            next_token(parser);
            left = (ASTNode*)array_lit;
            break;
        }
        case TOKEN_MINUS: // unary minus
        case TOKEN_NOT: // unary not
        case TOKEN_STAR: // dereference operator
        case TOKEN_AMPERSAND: // address operator
        case TOKEN_INCREMENT: // prefix increment
        case TOKEN_DECREMENT: { // prefix decrement
             TokenType op = parser->current_token->type;
             int op_line = parser->current_token->line;
             int op_column = parser->current_token->column;
             next_token(parser);
             ASTNode* right = parse_expression(parser, 5); // unary precedence
             UnaryExpressionNode* unary = ton_malloc(sizeof(UnaryExpressionNode));
             unary->type = NODE_UNARY_EXPRESSION;
             unary->line = op_line;
             unary->column = op_column;
             unary->operator = create_token(op, "", op_line, op_column);
             unary->operand = right;
             left = (ASTNode*)unary;
             break;
         }
         case TOKEN_TYPEOF: {
             int op_line = parser->current_token->line;
             int op_column = parser->current_token->column;
             next_token(parser); // consume 'typeof'
             expect_token(parser, TOKEN_LPAREN, "Expected '(' after 'typeof'");
             next_token(parser); // consume '('
             ASTNode* operand = parse_expression(parser, 0);
             expect_token(parser, TOKEN_RPAREN, "Expected ')' after typeof expression");
             next_token(parser); // consume ')'
             left = create_typeof_expression_node(operand, op_line, op_column);
             break;
         }
         case TOKEN_NEW: {
             int new_line = parser->current_token->line;
             int new_column = parser->current_token->column;
             next_token(parser); // consume 'new'
             
             expect_token(parser, TOKEN_IDENTIFIER, "Expected class name after 'new'");
             char* class_name = my_strdup_parser(parser->current_token->lexeme);
             next_token(parser); // consume class name
             
             // Parse arguments (optional)
             ASTNode** arguments = NULL;
             int num_arguments = 0;
             
             if (match_token(parser, TOKEN_LPAREN)) {
                 next_token(parser); // consume '('
                 
                 if (!match_token(parser, TOKEN_RPAREN)) {
                     int capacity = 4;
                     arguments = ton_malloc(sizeof(ASTNode*) * capacity);
                     
                     do {
                         if (num_arguments >= capacity) {
                             capacity *= 2;
                             arguments = ton_realloc(arguments, sizeof(ASTNode*) * capacity);
                         }
                         arguments[num_arguments++] = parse_expression(parser, 0);
                         
                         if (match_token(parser, TOKEN_COMMA)) {
                             next_token(parser);
                         } else {
                             break;
                         }
                     } while (parser->current_token->type != TOKEN_RPAREN);
                 }
                 
                 expect_token(parser, TOKEN_RPAREN, "Expected ')' after arguments");
                 next_token(parser); // consume ')'
             }
             
             left = create_new_expression_node(class_name, arguments, num_arguments, new_line, new_column);
             break;
         }
         case TOKEN_EOF:
             parser_error(parser, "Unexpected end of file in expression");
             break;
         default:
             parser_error(parser, "Unexpected token in expression");
     }

     while (true) {
        // Handle postfix increment/decrement operators
        if (match_token(parser, TOKEN_INCREMENT) || match_token(parser, TOKEN_DECREMENT)) {
            TokenType op = parser->current_token->type;
            int op_line = parser->current_token->line;
            int op_column = parser->current_token->column;
            next_token(parser);
            
            UnaryExpressionNode* unary = malloc(sizeof(UnaryExpressionNode));
            unary->type = NODE_UNARY_EXPRESSION;
            unary->line = op_line;
            unary->column = op_column;
            unary->operator = create_token(op, "", op_line, op_column);
            unary->operand = left;
            left = (ASTNode*)unary;
            continue;
        }
        
        // Handle array access first (highest precedence)
        if (match_token(parser, TOKEN_LBRACKET)) {
            next_token(parser); // consume '['
            ASTNode* index = parse_expression(parser, 0);
            expect_token(parser, TOKEN_RBRACKET, "Expected ']' after array index");
            next_token(parser);
            
            ArrayAccessExpressionNode* access = malloc(sizeof(ArrayAccessExpressionNode));
            access->base.type = NODE_ARRAY_ACCESS_EXPRESSION;
            access->base.line = parser->current_token->line;
            access->base.column = parser->current_token->column;
            access->array = left;
            access->index = index;
            left = (ASTNode*)access;
            continue;
        }
        
        // Handle member access (dot operator)
        if (match_token(parser, TOKEN_DOT)) {
            int dot_line = parser->current_token->line;
            int dot_column = parser->current_token->column;
            next_token(parser); // consume '.'
            
            expect_token(parser, TOKEN_IDENTIFIER, "Expected member name after '.'");
            char* member_name = my_strdup_parser(parser->current_token->lexeme);
            next_token(parser);
            
            MemberAccessExpressionNode* member_access = malloc(sizeof(MemberAccessExpressionNode));
            member_access->base.type = NODE_MEMBER_ACCESS_EXPRESSION;
            member_access->base.line = dot_line;
            member_access->base.column = dot_column;
            member_access->object = left;
            member_access->member = member_name;
            left = (ASTNode*)member_access;
            continue;
        }
        
        int prec = get_precedence(parser->current_token->type);
        if (prec == 0 || prec < min_precedence) break;

        // Handle ternary operator specially
        if (match_token(parser, TOKEN_QUESTION)) {
            int op_line = parser->current_token->line;
            int op_column = parser->current_token->column;
            next_token(parser); // consume '?'
            
            ASTNode* true_expr = parse_expression(parser, 0);
            expect_token(parser, TOKEN_COLON, "Expected ':' in ternary operator");
            next_token(parser); // consume ':'
            ASTNode* false_expr = parse_expression(parser, prec + 1);
            
            ConditionalExpressionNode* conditional = malloc(sizeof(ConditionalExpressionNode));
            conditional->type = NODE_CONDITIONAL_EXPRESSION;
            conditional->line = op_line;
            conditional->column = op_column;
            conditional->condition = left;
            conditional->true_expr = true_expr;
            conditional->false_expr = false_expr;
            left = (ASTNode*)conditional;
            continue;
        }

         TokenType op = parser->current_token->type;
         int op_line = parser->current_token->line;
         int op_column = parser->current_token->column;
         next_token(parser);
         ASTNode* right = parse_expression(parser, prec + 1);

         BinaryExpressionNode* bin = malloc(sizeof(BinaryExpressionNode));
         bin->type = NODE_BINARY_EXPRESSION;
         bin->line = op_line;
         bin->column = op_column;
         bin->operator = create_token(op, "", op_line, op_column);
         bin->left = left;
         bin->right = right;
         left = (ASTNode*)bin;
     }

     return left;
 }

 // ---------- FUNCTION CALL ----------
 ASTNode* parse_function_call_expression(Parser* parser, ASTNode* callee) {
     expect_token(parser, TOKEN_LPAREN, "Expected '(' for function call");
     next_token(parser);

     struct FunctionCallExpressionNode* call = malloc(sizeof(struct FunctionCallExpressionNode));
     call->base.type = NODE_FN_CALL_EXPRESSION;
     call->base.line = parser->current_token->line;
     call->base.column = parser->current_token->column;
     call->callee = callee;

     int capacity = 4;
     ASTNode** args = malloc(sizeof(ASTNode*) * capacity);
     int count = 0;

     while (!match_token(parser, TOKEN_RPAREN) && !match_token(parser, TOKEN_EOF)) {
         ASTNode* expr = parse_expression(parser, 0);
         if (count >= capacity) {
             capacity *= 2;
             args = realloc(args, sizeof(ASTNode*) * capacity);
         }
         args[count++] = expr;

         if (match_token(parser, TOKEN_COMMA)) next_token(parser);
         else break;
     }

     call->arguments = args;
     call->num_arguments = count;

     expect_token(parser, TOKEN_RPAREN, "Expected ')' after function arguments");
     next_token(parser);

     return (ASTNode*)call;
 }

 // ---------- TYPES ----------
 TypeNode* parse_type(Parser* parser) {
    TypeNode* type_node = malloc(sizeof(TypeNode));
    type_node->type = NODE_PARAMETER;
    type_node->line = parser->current_token->line;
    type_node->column = parser->current_token->column;

    if (match_token(parser, TOKEN_TYPE_INT)) type_node->var_type = VAR_TYPE_INT;
    else if (match_token(parser, TOKEN_TYPE_FLOAT)) type_node->var_type = VAR_TYPE_FLOAT;
    else if (match_token(parser, TOKEN_TYPE_STRING)) type_node->var_type = VAR_TYPE_STRING;
    else if (match_token(parser, TOKEN_TYPE_BOOL)) type_node->var_type = VAR_TYPE_BOOL;
    else if (match_token(parser, TOKEN_TYPE_VOID)) type_node->var_type = VAR_TYPE_VOID;
    else {
        free(type_node);
        return NULL;
    }

    next_token(parser);
    
    // Check for pointer type: int* or float*
    if (match_token(parser, TOKEN_STAR)) {
        type_node->var_type = VAR_TYPE_POINTER;
        next_token(parser); // consume '*'
    }
    // Check for array type: int[10] or int[]
    else if (match_token(parser, TOKEN_LBRACKET)) {
        type_node->var_type = VAR_TYPE_ARRAY;
        next_token(parser); // consume '['
        expect_token(parser, TOKEN_RBRACKET, "Expected ']' after array type");
        next_token(parser); // consume ']'
    }
    
    return type_node;
}
 // ---------- AST MEMORY MANAGEMENT ----------
 void free_ast(ASTNode* node) {
     if (!node) return;

     switch (node->type) {
         case NODE_PROGRAM: {
             ProgramNode* prog = (ProgramNode*)node;
             for (int i = 0; i < prog->num_statements; i++) {
                 free_ast(prog->statements[i]);
             }
             free(prog->statements);
             free(prog);
             break;
         }
         case NODE_VAR_DECLARATION: {
             VariableDeclarationNode* var = (VariableDeclarationNode*)node;
             if (var->identifier) free(var->identifier);
             if (var->initializer) free_ast(var->initializer);
             free(var);
             break;
         }
         case NODE_FN_DECLARATION: {
             FunctionDeclarationNode* fn = (FunctionDeclarationNode*)node;
             if (fn->identifier) free(fn->identifier->lexeme); // Assuming Token* lexeme copied
             free(fn->identifier);
             for (int i = 0; i < fn->num_parameters; i++) {
                 if (fn->parameters[i]->identifier) free(fn->parameters[i]->identifier->lexeme);
                 free(fn->parameters[i]->identifier);
                 free(fn->parameters[i]);
             }
             free(fn->parameters);
             free_ast((ASTNode*)fn->body);
             free(fn);
             break;
         }
         case NODE_BLOCK_STATEMENT: {
             BlockStatementNode* block = (BlockStatementNode*)node;
             for (int i = 0; i < block->num_statements; i++) {
                 free_ast(block->statements[i]);
             }
             free(block->statements);
             free(block);
             break;
         }
         case NODE_IF_STATEMENT: {
             IfStatementNode* ifstmt = (IfStatementNode*)node;
             free_ast(ifstmt->condition);
             free_ast((ASTNode*)ifstmt->consequence);
             if (ifstmt->alternative) free_ast((ASTNode*)ifstmt->alternative);
             free(ifstmt);
             break;
         }
         case NODE_LOOP_STATEMENT: {
             LoopStatementNode* loop = (LoopStatementNode*)node;
             if (loop->start_expr) free_ast(loop->start_expr);
             if (loop->end_expr) free_ast(loop->end_expr);
             if (loop->collection_expr) free_ast(loop->collection_expr);
             free_ast((ASTNode*)loop->body);
             free(loop);
             break;
         }
         case NODE_WHILE_STATEMENT: {
             WhileStatementNode* wh = (WhileStatementNode*)node;
             free_ast(wh->condition);
             free_ast((ASTNode*)wh->body);
             free(wh);
             break;
         }
         case NODE_SWITCH_STATEMENT: {
             SwitchStatementNode* switch_stmt = (SwitchStatementNode*)node;
             free_ast(switch_stmt->expression);
             for (int i = 0; i < switch_stmt->num_cases; i++) {
                 CaseStatementNode* case_stmt = switch_stmt->cases[i];
                 free_ast(case_stmt->value);
                 for (int j = 0; j < case_stmt->num_statements; j++) {
                     free_ast(case_stmt->statements[j]);
                 }
                 free(case_stmt->statements);
                 free(case_stmt);
             }
             free(switch_stmt->cases);
             if (switch_stmt->default_case) {
                 free_ast((ASTNode*)switch_stmt->default_case);
             }
             free(switch_stmt);
             break;
         }
         case NODE_CASE_STATEMENT: {
             CaseStatementNode* case_stmt = (CaseStatementNode*)node;
             free_ast(case_stmt->value);
             for (int i = 0; i < case_stmt->num_statements; i++) {
                 free_ast(case_stmt->statements[i]);
             }
             free(case_stmt->statements);
             free(case_stmt);
             break;
         }
         case NODE_BREAK_STATEMENT: {
             BreakStatementNode* break_stmt = (BreakStatementNode*)node;
             free(break_stmt);
             break;
         }
         case NODE_RETURN_STATEMENT: {
             ReturnStatementNode* ret = (ReturnStatementNode*)node;
             if (ret->expression) free_ast(ret->expression);
             free(ret);
             break;
         }
         case NODE_PRINT_STATEMENT: {
             struct PrintStatementNode* print = (struct PrintStatementNode*)node;
             free_ast(print->expression);
             free(print);
             break;
         }
         case NODE_EXPRESSION_STATEMENT: {
             ExpressionStatementNode* exprstmt = (ExpressionStatementNode*)node;
             free_ast(exprstmt->expression);
             free(exprstmt);
             break;
         }
         case NODE_LITERAL_EXPRESSION: {
             LiteralExpressionNode* lit = (LiteralExpressionNode*)node;
             if (lit->value) free(lit->value->lexeme);
             free(lit->value);
             free(lit);
             break;
         }
         case NODE_IDENTIFIER_EXPRESSION: {
             IdentifierExpressionNode* id = (IdentifierExpressionNode*)node;
             if (id->identifier) free(id->identifier);
             free(id);
             break;
         }
         case NODE_BINARY_EXPRESSION: {
             BinaryExpressionNode* bin = (BinaryExpressionNode*)node;
             free_ast(bin->left);
             free_ast(bin->right);
             free(bin);
             break;
         }
         case NODE_UNARY_EXPRESSION: {
             UnaryExpressionNode* unary = (UnaryExpressionNode*)node;
             free_ast(unary->operand);
             free(unary);
             break;
         }
         case NODE_FN_CALL_EXPRESSION: {
             FunctionCallExpressionNode* fncall = (FunctionCallExpressionNode*)node;
             free_ast(fncall->callee);
             for (int i = 0; i < fncall->num_arguments; i++) {
                 free_ast(fncall->arguments[i]);
             }
             free(fncall->arguments);
             free(fncall);
             break;
         }
         case NODE_MEMBER_ACCESS_EXPRESSION: {
             MemberAccessExpressionNode* member = (MemberAccessExpressionNode*)node;
             free_ast(member->object);
             if (member->member) free(member->member);
             free(member);
             break;
         }
         default:
             free(node);
             break;
     }
 }

 // ---------- DEBUG PRINT AST ----------
 void print_ast(ASTNode* node, int indent) {
     if (!node) return;
     for (int i = 0; i < indent; i++) printf("  ");

     switch (node->type) {
         case NODE_PROGRAM: {
             printf("Program\n");
             ProgramNode* prog = (ProgramNode*)node;
             for (int i = 0; i < prog->num_statements; i++)
                 print_ast(prog->statements[i], indent + 1);
             break;
         }
         case NODE_VAR_DECLARATION: {
             VariableDeclarationNode* var = (VariableDeclarationNode*)node;
             printf("VarDecl: %s\n", var->identifier);
             if (var->initializer) print_ast(var->initializer, indent + 1);
             break;
         }
         case NODE_FN_DECLARATION: {
             FunctionDeclarationNode* fn = (FunctionDeclarationNode*)node;
             printf("FnDecl: %s\n", fn->identifier->lexeme);
             for (int i = 0; i < fn->num_parameters; i++) {
                 for (int j = 0; j < indent + 1; j++) printf("  ");
                 printf("Param: %s\n", fn->parameters[i]->identifier->lexeme);
             }
             print_ast((ASTNode*)fn->body, indent + 1);
             break;
         }
         case NODE_BLOCK_STATEMENT: {
             printf("Block\n");
             BlockStatementNode* block = (BlockStatementNode*)node;
             for (int i = 0; i < block->num_statements; i++)
                 print_ast(block->statements[i], indent + 1);
             break;
         }
         case NODE_IF_STATEMENT: {
             IfStatementNode* ifstmt = (IfStatementNode*)node;
             printf("If\n");
             print_ast(ifstmt->condition, indent + 1);
             print_ast((ASTNode*)ifstmt->consequence, indent + 1);
             if (ifstmt->alternative) print_ast((ASTNode*)ifstmt->alternative, indent + 1);
             break;
         }
         case NODE_LOOP_STATEMENT: {
             printf("Loop\n");
             LoopStatementNode* loop = (LoopStatementNode*)node;
             print_ast((ASTNode*)loop->body, indent + 1);
             break;
         }
         case NODE_CONTINUE_STATEMENT: {
             printf("Continue\n");
             break;
         }
         case NODE_WHILE_STATEMENT: {
             printf("While\n");
             WhileStatementNode* wh = (WhileStatementNode*)node;
             print_ast(wh->condition, indent + 1);
             print_ast((ASTNode*)wh->body, indent + 1);
             break;
         }
         case NODE_RETURN_STATEMENT: {
             printf("Return\n");
             ReturnStatementNode* ret = (ReturnStatementNode*)node;
             if (ret->expression) print_ast(ret->expression, indent + 1);
             break;
         }
         case NODE_PRINT_STATEMENT: {
             printf("Print\n");
             struct PrintStatementNode* print = (struct PrintStatementNode*)node;
             print_ast(print->expression, indent + 1);
             break;
         }
         /* Continue statements are handled separately */
         case NODE_LITERAL_EXPRESSION: {
             LiteralExpressionNode* lit = (LiteralExpressionNode*)node;
             printf("Literal: %s\n", lit->value->lexeme);
             break;
         }
         case NODE_IDENTIFIER_EXPRESSION: {
             IdentifierExpressionNode* id = (IdentifierExpressionNode*)node;
             printf("Identifier: %s\n", id->identifier);
             break;
         }
         case NODE_BINARY_EXPRESSION: {
             BinaryExpressionNode* bin = (BinaryExpressionNode*)node;
             printf("BinaryExpr: %d\n", (int)bin->operator->type);
             print_ast(bin->left, indent + 1);
             print_ast(bin->right, indent + 1);
             break;
         }
         case NODE_UNARY_EXPRESSION: {
             UnaryExpressionNode* unary = (UnaryExpressionNode*)node;
             printf("UnaryExpr: %d\n", (int)unary->operator->type);
             print_ast(unary->operand, indent + 1);
             break;
         }
         case NODE_FN_CALL_EXPRESSION: {
             FunctionCallExpressionNode* fncall = (FunctionCallExpressionNode*)node;
             printf("FnCall\n");
             print_ast(fncall->callee, indent + 1);
             for (int i = 0; i < fncall->num_arguments; i++)
                 print_ast(fncall->arguments[i], indent + 1);
             break;
         }
         default:
             printf("Unknown node type %d\n", node->type);
     }
 } 

// Parse struct declaration: struct Point { x: int, y: int }
ASTNode* parse_struct_declaration(Parser* parser) {
    expect_token(parser, TOKEN_STRUCT, "Expected 'struct'");
    next_token(parser);
    
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        parser_error(parser, "Expected struct name");
        return NULL;
    }
    
    char* struct_name = my_strdup_parser(parser->current_token->lexeme);
    next_token(parser);
    
    expect_token(parser, TOKEN_LBRACE, "Expected '{'");
    next_token(parser);
    
    // Parse fields
    char** field_names = NULL;
    VariableType* field_types = NULL;
    int num_fields = 0;
    int capacity = 4;
    
    field_names = ton_malloc(capacity * sizeof(char*));
    field_types = ton_malloc(capacity * sizeof(VariableType));
    
    while (parser->current_token->type != TOKEN_RBRACE && parser->current_token->type != TOKEN_EOF) {
        if (num_fields >= capacity) {
            capacity *= 2;
            field_names = ton_realloc(field_names, capacity * sizeof(char*));
            field_types = ton_realloc(field_types, capacity * sizeof(VariableType));
        }
        
        // Parse field name
        if (parser->current_token->type != TOKEN_IDENTIFIER) {
            parser_error(parser, "Expected field name");
            free(struct_name);
            for (int i = 0; i < num_fields; i++) {
                free(field_names[i]);
            }
            free(field_names);
            free(field_types);
            return NULL;
        }
        
        field_names[num_fields] = my_strdup_parser(parser->current_token->lexeme);
        next_token(parser);
        
        expect_token(parser, TOKEN_COLON, "Expected ':'");
        next_token(parser);
        
        // Parse field type
        TypeNode* type_node = parse_type(parser);
        if (!type_node) {
            parser_error(parser, "Expected field type");
            free(struct_name);
            for (int i = 0; i <= num_fields; i++) {
                free(field_names[i]);
            }
            free(field_names);
            free(field_types);
            return NULL;
        }
        field_types[num_fields] = type_node->var_type;
        free(type_node);
        
        num_fields++;
        
        if (parser->current_token->type == TOKEN_SEMICOLON) {
            next_token(parser);
        }
        }
        
        expect_token(parser, TOKEN_RBRACE, "Expected '}'");
        next_token(parser);
        
        // Parse methods (optional)
        FunctionDeclarationNode** methods = NULL;
        int num_methods = 0;
        int method_capacity = 4;
        
        // Check if there are methods after the struct fields
        if (parser->current_token->type == TOKEN_LBRACE) {
            next_token(parser); // consume '{'
            
            methods = ton_malloc(method_capacity * sizeof(FunctionDeclarationNode*));
            
            while (parser->current_token->type != TOKEN_RBRACE && parser->current_token->type != TOKEN_EOF) {
                if (parser->current_token->type == TOKEN_FN) {
                    if (num_methods >= method_capacity) {
                        method_capacity *= 2;
                        methods = ton_realloc(methods, method_capacity * sizeof(FunctionDeclarationNode*));
                    }
                    
                    ASTNode* method = parse_function_declaration(parser);
                    if (method) {
                        methods[num_methods] = (FunctionDeclarationNode*)method;
                        num_methods++;
                    } else {
                        parser_error(parser, "Failed to parse method");
                        break;
                    }
                } else {
                    parser_error(parser, "Expected method declaration or '}'");
                    break;
                }
            }
            
            if (parser->current_token->type == TOKEN_RBRACE) {
                next_token(parser); // consume closing '}'
            }
        }
        
        struct StructDeclarationNode* struct_node = ton_malloc(sizeof(struct StructDeclarationNode));
        struct_node->type = NODE_STRUCT_DECLARATION;
        struct_node->line = parser->current_token->line;
        struct_node->column = parser->current_token->column;
        struct_node->name = struct_name;
        struct_node->field_names = field_names;
        struct_node->field_types = field_types;
        struct_node->num_fields = num_fields;
        struct_node->methods = methods;
        struct_node->num_methods = num_methods;
        
        return (ASTNode*)struct_node;
}

ASTNode* parse_import_statement(Parser* parser) {
    ImportStatementNode* import_node = ton_malloc(sizeof(ImportStatementNode));
    import_node->type = NODE_IMPORT_STATEMENT;
    import_node->line = parser->current_token->line;
    import_node->column = parser->current_token->column;
    
    expect_token(parser, TOKEN_IMPORT, "Expected 'import'");
    next_token(parser); // consume 'import'
    
    expect_token(parser, TOKEN_STRING_LITERAL, "Expected string literal for module path");
    import_node->module_path = my_strdup_parser(parser->current_token->lexeme);
    next_token(parser); // consume string literal
    
    expect_token(parser, TOKEN_SEMICOLON, "Expected ';' after import statement");
    next_token(parser); // consume ';'
    
    return (ASTNode*)import_node;
}

// Helper function to parse access modifier
static int parse_access_modifier(Parser* parser) {
    switch (parser->current_token->type) {
        case TOKEN_PUBLIC:
            next_token(parser);
            return ACCESS_PUBLIC;
        case TOKEN_PRIVATE:
            next_token(parser);
            return ACCESS_PRIVATE;
        case TOKEN_PROTECTED:
            next_token(parser);
            return ACCESS_PROTECTED;
        default:
            return ACCESS_PUBLIC; // default access
    }
}

// Parse class declaration: class Animal extends Object { ... }
ASTNode* parse_class_declaration(Parser* parser) {
    expect_token(parser, TOKEN_CLASS, "Expected 'class'");
    next_token(parser);
    
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        parser_error(parser, "Expected class name");
        return NULL;
    }
    
    char* class_name = my_strdup_parser(parser->current_token->lexeme);
    next_token(parser);
    
    // Parse inheritance (optional)
    char* parent_name = NULL;
    if (parser->current_token->type == TOKEN_EXTENDS) {
        next_token(parser); // consume 'extends'
        if (parser->current_token->type != TOKEN_IDENTIFIER) {
            parser_error(parser, "Expected parent class name after 'extends'");
            free(class_name);
            return NULL;
        }
        parent_name = my_strdup_parser(parser->current_token->lexeme);
        next_token(parser);
    }
    
    expect_token(parser, TOKEN_LBRACE, "Expected '{'");
    next_token(parser);
    
    // Parse fields and methods
    char** field_names = NULL;
    VariableType* field_types = NULL;
    int* field_access = NULL;
    int num_fields = 0;
    int field_capacity = 4;
    
    FunctionDeclarationNode** methods = NULL;
    int* method_access = NULL;
    int* method_flags = NULL;
    int num_methods = 0;
    int method_capacity = 4;
    
    field_names = malloc(field_capacity * sizeof(char*));
    field_types = malloc(field_capacity * sizeof(VariableType));
    field_access = malloc(field_capacity * sizeof(int));
    
    methods = malloc(method_capacity * sizeof(FunctionDeclarationNode*));
    method_access = malloc(method_capacity * sizeof(int));
    method_flags = malloc(method_capacity * sizeof(int));
    
    while (parser->current_token->type != TOKEN_RBRACE && parser->current_token->type != TOKEN_EOF) {
        // Parse access modifier
        int access = parse_access_modifier(parser);
        
        if (parser->current_token->type == TOKEN_FN) {
            // Parse method
            if (num_methods >= method_capacity) {
                method_capacity *= 2;
                methods = realloc(methods, method_capacity * sizeof(FunctionDeclarationNode*));
                method_access = realloc(method_access, method_capacity * sizeof(int));
                method_flags = realloc(method_flags, method_capacity * sizeof(int));
            }
            
            ASTNode* method = parse_function_declaration(parser);
            if (method) {
                methods[num_methods] = (FunctionDeclarationNode*)method;
                method_access[num_methods] = access;
                method_flags[num_methods] = 0; // TODO: parse virtual, override, etc.
                num_methods++;
            } else {
                parser_error(parser, "Failed to parse method");
                break;
            }
        } else if (parser->current_token->type == TOKEN_IDENTIFIER) {
            // Parse field
            if (num_fields >= field_capacity) {
                field_capacity *= 2;
                field_names = realloc(field_names, field_capacity * sizeof(char*));
                field_types = realloc(field_types, field_capacity * sizeof(VariableType));
                field_access = realloc(field_access, field_capacity * sizeof(int));
            }
            
            field_names[num_fields] = my_strdup_parser(parser->current_token->lexeme);
            next_token(parser);
            
            expect_token(parser, TOKEN_COLON, "Expected ':'");
            next_token(parser);
            
            TypeNode* type_node = parse_type(parser);
            if (!type_node) {
                parser_error(parser, "Expected field type");
                // Cleanup and return NULL
                free(class_name);
                if (parent_name) free(parent_name);
                for (int i = 0; i < num_fields; i++) free(field_names[i]);
                free(field_names);
                free(field_types);
                free(field_access);
                for (int i = 0; i < num_methods; i++) free_ast((ASTNode*)methods[i]);
                free(methods);
                free(method_access);
                free(method_flags);
                return NULL;
            }
            field_types[num_fields] = type_node->var_type;
            field_access[num_fields] = access;
            free(type_node);
            
            num_fields++;
            
            if (parser->current_token->type == TOKEN_SEMICOLON) {
                next_token(parser);
            }
        } else {
            parser_error(parser, "Expected field or method declaration");
            break;
        }
    }
    
    expect_token(parser, TOKEN_RBRACE, "Expected '}'");
    next_token(parser);
    
    // Create class declaration node
    ClassDeclarationNode* class_node = malloc(sizeof(ClassDeclarationNode));
    class_node->type = NODE_CLASS_DECLARATION;
    class_node->line = parser->current_token->line;
    class_node->column = parser->current_token->column;
    class_node->name = class_name;
    class_node->parent_name = parent_name;
    class_node->field_names = field_names;
    class_node->field_types = field_types;
    class_node->field_access = field_access;
    class_node->num_fields = num_fields;
    class_node->methods = methods;
    class_node->method_access = method_access;
    class_node->method_flags = method_flags;
    class_node->num_methods = num_methods;
    
    return (ASTNode*)class_node;
}

 // ---------- OPTIMIZACJE PRZYSZŁOŚCIOWE ----------
 /*
   1. Dodanie nowych typów prostych i strukturalnych (arrays, maps).
   2. Łatwe wprowadzanie nowych operatorów poprzez get_precedence().
   3. Rozszerzenie parsera o wyrażenia warunkowe inline, ternary, switch-case.
   4. Dodanie parsera importów i modułów.
   5. Wsparcie dla funkcji anonimowych (lambda) i closures.
   6. Debugowanie AST przy użyciu print_ast().
   7. Zarządzanie pamięcią przez free_ast() zapobiega wyciekom.
 */

