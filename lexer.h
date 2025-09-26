#ifndef LEXER_H
#define LEXER_H

#include "token.h"

// Lexer structure
typedef struct {
    const char* source;
    int source_length;
    int current_pos;
    int line;
    int column;
} Lexer;

// Function prototypes
void init_lexer(Lexer* lexer, const char* source);
Token* get_next_token(Lexer* lexer);
const char* token_type_to_string(TokenType type);

#endif // LEXER_H