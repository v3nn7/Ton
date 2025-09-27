#define _GNU_SOURCE
#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"

// Custom implementation of strndup for portability
static char* my_strndup(const char* s, size_t n) {
    size_t len = strnlen(s, n);
    char* d = (char*)malloc(len + 1);
    if (!d) return NULL;
    memcpy(d, s, len);
    d[len] = '\0';
    return d;
}

// Initialize the lexer with source code
void init_lexer(Lexer* lexer, const char* source) {
    lexer->source = source;
    lexer->source_length = strlen(source);
    lexer->current_pos = 0;
    lexer->line = 1;
    lexer->column = 1;
}

// Advance the lexer's position
static char advance(Lexer* lexer) {
    char c = lexer->source[lexer->current_pos];
    lexer->current_pos++;
    lexer->column++;
    return c;
}

// Peek at the next character without advancing
static char peek(Lexer* lexer) {
    if (lexer->current_pos >= lexer->source_length) {
        return '\0';
    }
    return lexer->source[lexer->current_pos];
}

// Skip whitespace and comments
static void skip_whitespace_and_comments(Lexer* lexer) {
    while (lexer->current_pos < lexer->source_length) {
        char c = peek(lexer);
        if (isspace(c)) {
            advance(lexer);
            if (c == '\n') {
                lexer->line++;
                lexer->column = 1;
            }
        } else if (c == '/' && peek(lexer) == '/') { // Single-line comment
            while (lexer->current_pos < lexer->source_length && peek(lexer) != '\n') {
                advance(lexer);
            }
        } else if (c == '/' && lexer->current_pos + 1 < lexer->source_length && lexer->source[lexer->current_pos + 1] == '*') { // Multi-line comment
            advance(lexer); // Consume '/'
            advance(lexer); // Consume '*'
            while (lexer->current_pos < lexer->source_length) {
                if (lexer->current_pos + 1 < lexer->source_length && 
                    peek(lexer) == '*' && lexer->source[lexer->current_pos + 1] == '/') {
                    advance(lexer); // Consume '*'
                    advance(lexer); // Consume '/'
                    break;
                }
                char current = peek(lexer);
                if (current == '\n') {
                    lexer->line++;
                    lexer->column = 0; // Will be incremented by advance()
                }
                advance(lexer);
            }
        } else {
            break;
        }
    }
}

// Get the next token from the source code
Token* get_next_token(Lexer* lexer) {
    skip_whitespace_and_comments(lexer);

    if (lexer->current_pos >= lexer->source_length) {
        return create_token(TOKEN_EOF, "", lexer->line, lexer->column);
    }

    int start_pos = lexer->current_pos;
    int start_column = lexer->column;
    char c = advance(lexer);

    // Handle identifiers and keywords
    if (isalpha(c) || c == '_') {
        while (isalnum(peek(lexer)) || peek(lexer) == '_') {
            advance(lexer);
        }
        int length = lexer->current_pos - start_pos;
        char* lexeme = my_strndup(lexer->source + start_pos, length);

        // Check for keywords
        if (strcmp(lexeme, "let") == 0) return create_token(TOKEN_LET, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "const") == 0) return create_token(TOKEN_CONST, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "fn") == 0) return create_token(TOKEN_FN, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "if") == 0) return create_token(TOKEN_IF, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "else") == 0) return create_token(TOKEN_ELSE, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "loop") == 0) return create_token(TOKEN_LOOP, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "from") == 0) return create_token(TOKEN_FROM, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "to") == 0) return create_token(TOKEN_TO, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "in") == 0) return create_token(TOKEN_IN, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "return") == 0) return create_token(TOKEN_RETURN, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "var") == 0) return create_token(TOKEN_VAR, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "while") == 0) return create_token(TOKEN_WHILE, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "for") == 0) return create_token(TOKEN_FOR, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "print") == 0) return create_token(TOKEN_PRINT, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "switch") == 0) return create_token(TOKEN_SWITCH, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "case") == 0) return create_token(TOKEN_CASE, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "default") == 0) return create_token(TOKEN_DEFAULT, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "break") == 0) return create_token(TOKEN_BREAK, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "continue") == 0) return create_token(TOKEN_CONTINUE, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "struct") == 0) return create_token(TOKEN_STRUCT, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "class") == 0) return create_token(TOKEN_CLASS, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "extends") == 0) return create_token(TOKEN_EXTENDS, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "this") == 0) return create_token(TOKEN_THIS, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "import") == 0) return create_token(TOKEN_IMPORT, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "macro") == 0) return create_token(TOKEN_MACRO, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "public") == 0) return create_token(TOKEN_PUBLIC, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "private") == 0) return create_token(TOKEN_PRIVATE, lexeme, lexer->line, start_column);
        // if (strcmp(lexeme, "read_line") == 0) return create_token(TOKEN_READ_LINE, lexeme, lexer->line, start_column); // Removed, handled as IDENTIFIER
        if (strcmp(lexeme, "true") == 0) return create_token(TOKEN_TRUE, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "false") == 0) return create_token(TOKEN_FALSE, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "typeof") == 0) return create_token(TOKEN_TYPEOF, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "sizeof") == 0) return create_token(TOKEN_SIZEOF, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "alignof") == 0) return create_token(TOKEN_ALIGNOF, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "new") == 0) return create_token(TOKEN_NEW, lexeme, lexer->line, start_column);
        if (strcmp(lexeme, "null") == 0) return create_token(TOKEN_NULL, lexeme, lexer->line, start_column);

        // Types
        if (strcmp(lexeme, "int") == 0) return create_token(TOKEN_TYPE_INT, lexeme, lexer->line, start_column);
        else if (strcmp(lexeme, "float") == 0) return create_token(TOKEN_TYPE_FLOAT, lexeme, lexer->line, start_column);
        else if (strcmp(lexeme, "bool") == 0) return create_token(TOKEN_TYPE_BOOL, lexeme, lexer->line, start_column);
        else if (strcmp(lexeme, "string") == 0) return create_token(TOKEN_TYPE_STRING, lexeme, lexer->line, start_column);
        else if (strcmp(lexeme, "void") == 0) return create_token(TOKEN_TYPE_VOID, lexeme, lexer->line, start_column);
        else if (strcmp(lexeme, "char") == 0) return create_token(TOKEN_TYPE_CHAR, lexeme, lexer->line, start_column);

        return create_token(TOKEN_IDENTIFIER, lexeme, lexer->line, start_column);
    }

    // Handle integer and float literals
    if (isdigit(c)) {
        // Check for hexadecimal literals (0x...)
        if (c == '0' && (peek(lexer) == 'x' || peek(lexer) == 'X')) {
            advance(lexer); // consume 'x' or 'X'
            if (!isxdigit(peek(lexer))) {
                fprintf(stderr, "Lexing Error: Invalid hexadecimal literal at line %d, column %d\n", lexer->line, start_column);
                return create_token(TOKEN_ERROR, NULL, lexer->line, start_column);
            }
            while (isxdigit(peek(lexer))) {
                advance(lexer);
            }
            int length = lexer->current_pos - start_pos;
            char* lexeme = my_strndup(lexer->source + start_pos, length);
            return create_token(TOKEN_INT_LITERAL, lexeme, lexer->line, start_column);
        }
        // Check for binary literals (0b...)
        else if (c == '0' && (peek(lexer) == 'b' || peek(lexer) == 'B')) {
            advance(lexer); // consume 'b' or 'B'
            if (peek(lexer) != '0' && peek(lexer) != '1') {
                fprintf(stderr, "Lexing Error: Invalid binary literal at line %d, column %d\n", lexer->line, start_column);
                return create_token(TOKEN_ERROR, NULL, lexer->line, start_column);
            }
            while (peek(lexer) == '0' || peek(lexer) == '1') {
                advance(lexer);
            }
            int length = lexer->current_pos - start_pos;
            char* lexeme = my_strndup(lexer->source + start_pos, length);
            return create_token(TOKEN_INT_LITERAL, lexeme, lexer->line, start_column);
        }
        // Regular decimal numbers
        else {
            while (isdigit(peek(lexer))) {
                advance(lexer);
            }
            if (peek(lexer) == '.') {
                advance(lexer); // Consume the dot
                while (isdigit(peek(lexer))) {
                    advance(lexer);
                }
                int length = lexer->current_pos - start_pos;
                char* lexeme = my_strndup(lexer->source + start_pos, length);
                return create_token(TOKEN_FLOAT_LITERAL, lexeme, lexer->line, start_column);
            } else {
                int length = lexer->current_pos - start_pos;
                char* lexeme = my_strndup(lexer->source + start_pos, length);
                return create_token(TOKEN_INT_LITERAL, lexeme, lexer->line, start_column);
            }
        }
    }

    // Handle string literals (both regular and multiline)
    if (c == '"') {
        // Check for multiline string (triple quotes)
        if (peek(lexer) == '"' && lexer->current_pos + 1 < lexer->source_length && lexer->source[lexer->current_pos + 1] == '"') {
            // Multiline string with """
            advance(lexer); // consume second "
            advance(lexer); // consume third "
            
            char* processed_string = (char*)malloc(lexer->source_length);
            int processed_index = 0;
            
            while (lexer->current_pos + 2 < lexer->source_length) {
                // Check for closing """
                if (peek(lexer) == '"' && lexer->source[lexer->current_pos + 1] == '"' && lexer->source[lexer->current_pos + 2] == '"') {
                    advance(lexer); // consume first "
                    advance(lexer); // consume second "
                    advance(lexer); // consume third "
                    break;
                }
                
                char current_char = advance(lexer);
                
                // Handle escape sequences in multiline strings
                if (current_char == '\\') {
                    if (lexer->current_pos >= lexer->source_length) {
                        fprintf(stderr, "Lexing Error: Unterminated escape sequence in multiline string at line %d, column %d\n", lexer->line, lexer->column);
                        free(processed_string);
                        return create_token(TOKEN_ERROR, NULL, lexer->line, start_column);
                    }
                    
                    char escape_char = advance(lexer);
                    switch (escape_char) {
                        case 'n':
                            processed_string[processed_index++] = '\n';
                            break;
                        case 't':
                            processed_string[processed_index++] = '\t';
                            break;
                        case 'r':
                            processed_string[processed_index++] = '\r';
                            break;
                        case '\\':
                            processed_string[processed_index++] = '\\';
                            break;
                        case '"':
                            processed_string[processed_index++] = '"';
                            break;
                        case '0':
                            processed_string[processed_index++] = '\0';
                            break;
                        default:
                            processed_string[processed_index++] = '\\';
                            processed_string[processed_index++] = escape_char;
                            break;
                    }
                } else {
                    processed_string[processed_index++] = current_char;
                }
            }
            
            // Check if we found the closing """
            if (lexer->current_pos >= lexer->source_length || 
                !(lexer->source[lexer->current_pos - 3] == '"' && 
                  lexer->source[lexer->current_pos - 2] == '"' && 
                  lexer->source[lexer->current_pos - 1] == '"')) {
                fprintf(stderr, "Lexing Error: Unterminated multiline string literal at line %d, column %d\n", lexer->line, start_column);
                free(processed_string);
                return create_token(TOKEN_ERROR, NULL, lexer->line, start_column);
            }
            
            processed_string[processed_index] = '\0';
            
            // Create the final lexeme with quotes around the processed content
            int final_length = processed_index + 2;
            char* final_lexeme = (char*)malloc(final_length + 1);
            final_lexeme[0] = '"';
            memcpy(final_lexeme + 1, processed_string, processed_index);
            final_lexeme[final_length - 1] = '"';
            final_lexeme[final_length] = '\0';
            
            free(processed_string);
            return create_token(TOKEN_STRING_LITERAL, final_lexeme, lexer->line, start_column);
        }
        
        // Regular string literal
        // Create a buffer for the processed string (without quotes and with escape sequences processed)
        char* processed_string = (char*)malloc(lexer->source_length); // Allocate enough space
        int processed_index = 0;
        
        while (peek(lexer) != '"') {
            if (lexer->current_pos >= lexer->source_length) {
                fprintf(stderr, "Lexing Error: Unterminated string literal at line %d, column %d\n", lexer->line, start_column);
                free(processed_string);
                return create_token(TOKEN_ERROR, NULL, lexer->line, start_column);
            }
            
            char current_char = advance(lexer);
            
            // Handle escape sequences
            if (current_char == '\\') {
                if (lexer->current_pos >= lexer->source_length) {
                    fprintf(stderr, "Lexing Error: Unterminated escape sequence at line %d, column %d\n", lexer->line, lexer->column);
                    free(processed_string);
                    return create_token(TOKEN_ERROR, NULL, lexer->line, start_column);
                }
                
                char escape_char = advance(lexer);
                switch (escape_char) {
                    case 'n':
                        processed_string[processed_index++] = '\n';
                        break;
                    case 't':
                        processed_string[processed_index++] = '\t';
                        break;
                    case 'r':
                        processed_string[processed_index++] = '\r';
                        break;
                    case '\\':
                        processed_string[processed_index++] = '\\';
                        break;
                    case '"':
                        processed_string[processed_index++] = '"';
                        break;
                    case '0':
                        processed_string[processed_index++] = '\0';
                        break;
                    default:
                        // For unrecognized escape sequences, keep the backslash and character
                        processed_string[processed_index++] = '\\';
                        processed_string[processed_index++] = escape_char;
                        break;
                }
            } else {
                processed_string[processed_index++] = current_char;
            }
        }
        
        advance(lexer); // Consume the closing quote
        processed_string[processed_index] = '\0'; // Null-terminate the string
        
        // Create the final lexeme with quotes around the processed content
        int final_length = processed_index + 2; // +2 for quotes
        char* final_lexeme = (char*)malloc(final_length + 1);
        final_lexeme[0] = '"';
        memcpy(final_lexeme + 1, processed_string, processed_index);
        final_lexeme[final_length - 1] = '"';
        final_lexeme[final_length] = '\0';
        
        free(processed_string);
        return create_token(TOKEN_STRING_LITERAL, final_lexeme, lexer->line, start_column);
    }

    // Handle char literals
    if (c == '\'') {
        start_pos = lexer->current_pos;
        char char_val;

        if (peek(lexer) == '\'') {
            fprintf(stderr, "Lexing Error: Empty character literal at line %d, column %d\n", lexer->line, start_column);
            return create_token(TOKEN_ERROR, NULL, lexer->line, start_column);
        }

        if (peek(lexer) == '\\') {
            advance(lexer); // consume backslash
            char escape_char = advance(lexer);
            switch (escape_char) {
                case 'n': char_val = '\n'; break;
                case 't': char_val = '\t'; break;
                case 'r': char_val = '\r'; break;
                case '\\': char_val = '\\'; break;
                case '\'': char_val = '\''; break;
                case '0': char_val = '\0'; break;
                default:
                    fprintf(stderr, "Lexing Error: Unknown escape sequence '\\%c' in character literal at line %d, column %d\n", escape_char, lexer->line, start_column);
                    return create_token(TOKEN_ERROR, NULL, lexer->line, start_column);
            }
        } else {
            char_val = advance(lexer);
        }

        if (peek(lexer) != '\'') {
            fprintf(stderr, "Lexing Error: Unterminated or multi-character literal at line %d, column %d\n", lexer->line, start_column);
            return create_token(TOKEN_ERROR, NULL, lexer->line, start_column);
        }
        advance(lexer); // consume closing quote

        char* lexeme = (char*)malloc(2);
        lexeme[0] = char_val;
        lexeme[1] = '\0';
        return create_token(TOKEN_CHAR_LITERAL, lexeme, lexer->line, start_column);
    }

    // Handle single-character tokens and multi-character operators
    switch (c) {
        case '(': return create_token(TOKEN_LPAREN, "(", lexer->line, start_column);
        case ')': return create_token(TOKEN_RPAREN, ")", lexer->line, start_column);
        case '{': return create_token(TOKEN_LBRACE, "{", lexer->line, start_column);
        case '}': return create_token(TOKEN_RBRACE, "}", lexer->line, start_column);
        case '[': return create_token(TOKEN_LBRACKET, "[", lexer->line, start_column);
        case ']': return create_token(TOKEN_RBRACKET, "]", lexer->line, start_column);
        case ',': return create_token(TOKEN_COMMA, ",", lexer->line, start_column);
        case ':': return create_token(TOKEN_COLON, ":", lexer->line, start_column);
        case ';': return create_token(TOKEN_SEMICOLON, ";", lexer->line, start_column);
        case '.': return create_token(TOKEN_DOT, ".", lexer->line, start_column);
        case '%':
            if (peek(lexer) == '=') {
                advance(lexer);
                return create_token(TOKEN_MODULO_ASSIGN, "%=", lexer->line, start_column);
            }
            return create_token(TOKEN_MODULO, "%", lexer->line, start_column);
        case '+':
            if (peek(lexer) == '=') {
                advance(lexer);
                return create_token(TOKEN_PLUS_ASSIGN, "+=", lexer->line, start_column);
            }
            if (peek(lexer) == '+') {
                advance(lexer);
                return create_token(TOKEN_INCREMENT, "++", lexer->line, start_column);
            }
            return create_token(TOKEN_PLUS, "+", lexer->line, start_column);
        case '-':
            if (peek(lexer) == '>') {
                advance(lexer);
                return create_token(TOKEN_ARROW, "->", lexer->line, start_column);
            }
            if (peek(lexer) == '=') {
                advance(lexer);
                return create_token(TOKEN_MINUS_ASSIGN, "-=", lexer->line, start_column);
            }
            if (peek(lexer) == '-') {
                advance(lexer);
                return create_token(TOKEN_DECREMENT, "--", lexer->line, start_column);
            }
            return create_token(TOKEN_MINUS, "-", lexer->line, start_column);
        case '*':
            if (peek(lexer) == '=') {
                advance(lexer);
                return create_token(TOKEN_STAR_ASSIGN, "*=", lexer->line, start_column);
            }
            return create_token(TOKEN_STAR, "*", lexer->line, start_column);
        case '/':
            if (peek(lexer) == '=') {
                advance(lexer);
                return create_token(TOKEN_SLASH_ASSIGN, "/=", lexer->line, start_column);
            }
            return create_token(TOKEN_SLASH, "/", lexer->line, start_column);
        case '=':
            if (peek(lexer) == '=') {
                advance(lexer);
                return create_token(TOKEN_EQ, "==", lexer->line, start_column);
            }
            return create_token(TOKEN_ASSIGN, "=", lexer->line, start_column);
        case '>':
            if (peek(lexer) == '=') {
                advance(lexer);
                return create_token(TOKEN_GE, ">=", lexer->line, start_column);
            }
            return create_token(TOKEN_GT, ">", lexer->line, start_column);
        case '<':
            if (peek(lexer) == '=') {
                advance(lexer);
                return create_token(TOKEN_LE, "<=", lexer->line, start_column);
            }
            return create_token(TOKEN_LT, "<", lexer->line, start_column);
        case '!':
            if (peek(lexer) == '=') {
                advance(lexer);
                return create_token(TOKEN_NEQ, "!=", lexer->line, start_column);
            }
            return create_token(TOKEN_NOT, "!", lexer->line, start_column);
        case '&':
            if (peek(lexer) == '&') {
                advance(lexer);
                return create_token(TOKEN_AND, "&&", lexer->line, start_column);
            }
            return create_token(TOKEN_AMPERSAND, "&", lexer->line, start_column);
        case '|':
            if (peek(lexer) == '|') {
                advance(lexer);
                return create_token(TOKEN_OR, "||", lexer->line, start_column);
            }
            break; // Handle single | as error or future bitwise OR
        case '~':
            return create_token(TOKEN_TILDE, "~", lexer->line, start_column);
        case '?':
            return create_token(TOKEN_QUESTION, "?", lexer->line, start_column);
    }

    // If we reach here, it's an unrecognized character
    char error_lexeme[2] = {c, '\0'};
    return create_token(TOKEN_ERROR, error_lexeme, lexer->line, start_column);
}

const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_INT_LITERAL: return "INT_LITERAL";
        case TOKEN_FLOAT_LITERAL: return "FLOAT_LITERAL";
        case TOKEN_STRING_LITERAL: return "STRING_LITERAL";
        case TOKEN_CHAR_LITERAL: return "CHAR_LITERAL";
        case TOKEN_LET: return "LET";
        case TOKEN_CONST: return "CONST";
        case TOKEN_FN: return "FN";
        case TOKEN_IF: return "IF";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_LOOP: return "LOOP";
        case TOKEN_FROM: return "FROM";
        case TOKEN_TO: return "TO";
        case TOKEN_IN: return "IN";
        case TOKEN_RETURN: return "RETURN";
        case TOKEN_VAR: return "VAR";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_FOR: return "FOR";
        case TOKEN_PRINT: return "PRINT";
        case TOKEN_SWITCH: return "SWITCH";
        case TOKEN_CASE: return "CASE";
        case TOKEN_DEFAULT: return "DEFAULT";
        case TOKEN_BREAK: return "BREAK";
        case TOKEN_CONTINUE: return "CONTINUE";
        case TOKEN_STRUCT: return "STRUCT";
        case TOKEN_CLASS: return "CLASS";
        case TOKEN_THIS: return "THIS";
        case TOKEN_IMPORT: return "IMPORT";
        case TOKEN_PUBLIC: return "PUBLIC";
        case TOKEN_PRIVATE: return "PRIVATE";
        // case TOKEN_READ_LINE: return "READ_LINE"; // Removed, as TOKEN_READ_LINE is no longer a distinct token type
        case TOKEN_TRUE: return "TRUE";
        case TOKEN_FALSE: return "FALSE";
        case TOKEN_TYPEOF: return "TYPEOF";
        case TOKEN_TYPE_INT: return "TYPE_INT";
        case TOKEN_TYPE_FLOAT: return "TYPE_FLOAT";
        case TOKEN_TYPE_BOOL: return "TYPE_BOOL";
        case TOKEN_TYPE_STRING: return "TYPE_STRING";
        case TOKEN_TYPE_CHAR: return "TYPE_CHAR";
        case TOKEN_TYPE_VOID: return "TYPE_VOID";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_PLUS_ASSIGN: return "PLUS_ASSIGN";
        case TOKEN_MINUS_ASSIGN: return "MINUS_ASSIGN";
        case TOKEN_STAR_ASSIGN: return "STAR_ASSIGN";
        case TOKEN_SLASH_ASSIGN: return "SLASH_ASSIGN";
        case TOKEN_MODULO_ASSIGN: return "MODULO_ASSIGN";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_STAR: return "STAR";
        case TOKEN_SLASH: return "SLASH";
        case TOKEN_MODULO: return "MODULO";
        case TOKEN_INCREMENT: return "INCREMENT";
        case TOKEN_DECREMENT: return "DECREMENT";
        case TOKEN_EQ: return "EQ";
        case TOKEN_NEQ: return "NEQ";
        case TOKEN_LT: return "LT";
        case TOKEN_GT: return "GT";
        case TOKEN_LE: return "LE";
        case TOKEN_GE: return "GE";
        case TOKEN_AND: return "AND";
        case TOKEN_OR: return "OR";
        case TOKEN_NOT: return "NOT";
        case TOKEN_AMPERSAND: return "AMPERSAND";
        case TOKEN_QUESTION: return "QUESTION";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE";
        case TOKEN_LBRACKET: return "LBRACKET";
        case TOKEN_RBRACKET: return "RBRACKET";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_COLON: return "COLON";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_DOT: return "DOT";
        case TOKEN_ARROW: return "ARROW";
        case TOKEN_COMMENT: return "COMMENT";
        case TOKEN_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}