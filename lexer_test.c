#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", filename);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char* source_code = (char*)malloc(file_size + 1);
    if (source_code == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(file);
        return 1;
    }

    size_t bytes_read = fread(source_code, 1, file_size, file);
    if (bytes_read != (size_t)file_size) {
        fprintf(stderr, "Error reading file \"%s\".\n", filename);
        free(source_code);
        fclose(file);
        return 1;
    }
    source_code[file_size] = '\0';
    fclose(file);

    Lexer lexer;
    init_lexer(&lexer, source_code);

    Token* token;
    while ((token = get_next_token(&lexer))->type != TOKEN_EOF) {
        printf("Token: %-20s Lexeme: %s (Line: %d, Column: %d)\n",
               token_type_to_string(token->type),
               token->lexeme,
               token->line,
               token->column);
        free_token(token);
    }
    
    printf("Token: %-20s Lexeme: %s (Line: %d, Column: %d)\n",
           token_type_to_string(token->type),
           token->lexeme,
           token->line,
           token->column);
    free_token(token);

    free(source_code);
    return 0;
}