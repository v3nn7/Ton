// Exception handling parsing functions for Ton language
#include "parser.h"
#include "ast.h"
#include "memory.h"
#include <stdlib.h>
#include <string.h>

// Forward declaration
static char* my_strdup_parser(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char* copy = ton_malloc(len + 1);
    strcpy(copy, s);
    return copy;
}

ASTNode* parse_try_statement(Parser* parser) {
    expect_token(parser, TOKEN_TRY, "Expected 'try'");
    int line = parser->current_token->line;
    int column = parser->current_token->column;
    next_token(parser);

    // Parse try block
    BlockStatementNode* try_block = (BlockStatementNode*)parse_block_statement(parser);
    if (!try_block) {
        parser_error(parser, "Expected try block");
        return NULL;
    }

    // Parse catch blocks
    int catch_capacity = 5;
    CatchStatementNode** catch_blocks = ton_malloc(sizeof(CatchStatementNode*) * catch_capacity);
    int num_catch_blocks = 0;

    while (match_token(parser, TOKEN_CATCH)) {
        ASTNode* catch_node = parse_catch_statement(parser);
        if (catch_node) {
            if (num_catch_blocks >= catch_capacity) {
                catch_capacity *= 2;
                catch_blocks = ton_realloc(catch_blocks, sizeof(CatchStatementNode*) * catch_capacity);
            }
            catch_blocks[num_catch_blocks] = (CatchStatementNode*)catch_node;
            num_catch_blocks++;
        }
    }

    // Parse optional finally block
    FinallyStatementNode* finally_block = NULL;
    if (match_token(parser, TOKEN_FINALLY)) {
        finally_block = (FinallyStatementNode*)parse_finally_statement(parser);
    }

    // Must have at least one catch or finally block
    if (num_catch_blocks == 0 && !finally_block) {
        parser_error(parser, "Try statement must have at least one catch or finally block");
        free_ast_node((ASTNode*)try_block);
        free(catch_blocks);
        return NULL;
    }

    TryStatementNode* try_stmt = (TryStatementNode*)create_ast_node(NODE_TRY_STATEMENT, sizeof(TryStatementNode), line, column);
    try_stmt->try_block = try_block;
    try_stmt->catch_blocks = catch_blocks;
    try_stmt->num_catch_blocks = num_catch_blocks;
    try_stmt->finally_block = finally_block;

    return (ASTNode*)try_stmt;
}

ASTNode* parse_catch_statement(Parser* parser) {
    expect_token(parser, TOKEN_CATCH, "Expected 'catch'");
    int line = parser->current_token->line;
    int column = parser->current_token->column;
    next_token(parser);

    char* exception_type = NULL;
    char* exception_var = NULL;

    // Parse optional catch parameters: catch (ExceptionType varName) or catch (varName)
    if (match_token(parser, TOKEN_LPAREN)) {
        next_token(parser);

        if (parser->current_token->type == TOKEN_IDENTIFIER) {
            // Could be either type or variable name
            char* first_identifier = my_strdup_parser(parser->current_token->lexeme);
            next_token(parser);

            if (parser->current_token->type == TOKEN_IDENTIFIER) {
                // First was type, second is variable name
                exception_type = first_identifier;
                exception_var = my_strdup_parser(parser->current_token->lexeme);
                next_token(parser);
            } else {
                // Only variable name provided
                exception_var = first_identifier;
            }
        }

        expect_token(parser, TOKEN_RPAREN, "Expected ')' after catch parameters");
        next_token(parser);
    }

    // Parse catch block
    BlockStatementNode* catch_block = (BlockStatementNode*)parse_block_statement(parser);
    if (!catch_block) {
        parser_error(parser, "Expected catch block");
        if (exception_type) free(exception_type);
        if (exception_var) free(exception_var);
        return NULL;
    }

    CatchStatementNode* catch_stmt = (CatchStatementNode*)create_ast_node(NODE_CATCH_STATEMENT, sizeof(CatchStatementNode), line, column);
    catch_stmt->exception_type = exception_type;
    catch_stmt->exception_var = exception_var;
    catch_stmt->catch_block = catch_block;

    return (ASTNode*)catch_stmt;
}

ASTNode* parse_finally_statement(Parser* parser) {
    expect_token(parser, TOKEN_FINALLY, "Expected 'finally'");
    int line = parser->current_token->line;
    int column = parser->current_token->column;
    next_token(parser);

    // Parse finally block
    BlockStatementNode* finally_block = (BlockStatementNode*)parse_block_statement(parser);
    if (!finally_block) {
        parser_error(parser, "Expected finally block");
        return NULL;
    }

    FinallyStatementNode* finally_stmt = (FinallyStatementNode*)create_ast_node(NODE_FINALLY_STATEMENT, sizeof(FinallyStatementNode), line, column);
    finally_stmt->finally_block = finally_block;

    return (ASTNode*)finally_stmt;
}

ASTNode* parse_throw_statement(Parser* parser) {
    expect_token(parser, TOKEN_THROW, "Expected 'throw'");
    int line = parser->current_token->line;
    int column = parser->current_token->column;
    next_token(parser);

    // Parse exception expression
    ASTNode* exception_expr = parse_expression(parser, 0);
    if (!exception_expr) {
        parser_error(parser, "Expected expression after 'throw'");
        return NULL;
    }

    expect_token(parser, TOKEN_SEMICOLON, "Expected ';' after throw statement");
    next_token(parser);

    ThrowStatementNode* throw_stmt = (ThrowStatementNode*)create_ast_node(NODE_THROW_STATEMENT, sizeof(ThrowStatementNode), line, column);
    throw_stmt->exception_expr = exception_expr;

    return (ASTNode*)throw_stmt;
}