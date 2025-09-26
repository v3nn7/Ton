#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct {
    Lexer* lexer;
    Token* current_token;
    Token* peek_token;
} Parser;

void init_parser(Parser* parser, Lexer* lexer);
ASTNode* parse_program(Parser* parser);

// Helper for error reporting
void parser_error(Parser* parser, const char* msg);

// Forward declarations for parsing functions
ASTNode* parse_statement(Parser* parser);
ASTNode* parse_variable_declaration(Parser* parser);
ASTNode* parse_function_declaration(Parser* parser);
ASTNode* parse_struct_declaration(Parser* parser);
ASTNode* parse_import_statement(Parser* parser);
ASTNode* parse_if_statement(Parser* parser);
ASTNode* parse_loop_statement(Parser* parser);
ASTNode* parse_for_statement(Parser* parser);
ASTNode* parse_while_statement(Parser* parser);
ASTNode* parse_switch_statement(Parser* parser);
ASTNode* parse_break_statement(Parser* parser);
ASTNode* parse_continue_statement(Parser* parser);
ASTNode* parse_return_statement(Parser* parser);
ASTNode* parse_print_statement(Parser* parser); // Add this line
ASTNode* parse_expression_statement(Parser* parser);
ASTNode* parse_array_literal_expression(Parser* parser);
ASTNode* parse_array_access_expression(Parser* parser, ASTNode* array);
ASTNode* parse_expression(Parser* parser, int precedence);
ASTNode* parse_function_call_expression(Parser* parser, ASTNode* callee);
TypeNode* parse_type(Parser* parser);

#endif // PARSER_H