#include "token.h"
#include <stdlib.h>
#include <string.h>

// Safe local strdup replacement for tokens
static char* my_strdup_token(const char* s) {
    if (s == NULL) return NULL;
    size_t len = strlen(s) + 1;
    char* d = (char*)malloc(len);
    if (!d) return NULL;
    memcpy(d, s, len);
    return d;
}

// Helper function to create a new token
Token* create_token(TokenType type, const char* lexeme, int line, int column) {
    Token* token = (Token*)malloc(sizeof(Token));
    if (token == NULL) {
        perror("Failed to allocate token");
        exit(1);
    }
    token->type = type;
    token->lexeme = (lexeme != NULL) ? my_strdup_token(lexeme) : NULL;
    token->line = line;
    token->column = column;
    return token;
}

void free_token(Token* token) {
    if (token == NULL) return;
    free(token->lexeme);
    free(token);
}