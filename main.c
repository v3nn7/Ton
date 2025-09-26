#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "interpreter.h" // Include the interpreter header
#include "interpreter_core.h" // for register_builtins

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

    Parser parser;
    init_parser(&parser, &lexer);

    // Parse the source code
    ASTNode* program_ast = parse_program(&parser);

    // Create a global environment
    Environment* global_env = create_environment();

    // Register built-in functions into the global environment
    register_builtins(global_env);

    int program_exit_code = 0;

    // Interpret the AST (top-level statements)
    interpret_ast(program_ast, global_env);

    // After interpreting the entire AST, find and optionally execute the 'main' function
    Function* main_func = env_get_function(global_env, "main");
    if (main_func != NULL) {
        // Create a new environment for the main function's local scope
        Environment* main_env = create_child_environment(global_env);

        // Interpret the body of the main function
        // Execute the program
        Value result = interpret_ast((ASTNode*)main_func->body, main_env);
        
        // Set exit code based on result
        if (result.type == VALUE_INT) {
            program_exit_code = result.data.int_val;
        } else {
            // If main returns non-int or has no explicit return, default to 0
            program_exit_code = 0;
        }

        // Free the main function's local environment
        destroy_environment(main_env);
    } else {
        // Script mode: no main function defined; execution already happened at top level
        program_exit_code = 0;
    }

#ifdef TON_DEBUG
    fprintf(stderr, "DEBUG: Before freeing AST.\n");
#endif
    // Free the AST
    free_ast_node(program_ast);
#ifdef TON_DEBUG
    fprintf(stderr, "DEBUG: Before freeing global environment.\n");
#endif
    // Free the interpreter environment
    destroy_environment(global_env);
#ifdef TON_DEBUG
    fprintf(stderr, "DEBUG: Before freeing source code.\n");
#endif
    // Free the source code buffer
    free(source_code);
#ifdef TON_DEBUG
    fprintf(stderr, "DEBUG: Before freeing parser tokens.\n");
#endif
    // Free the last peek_token from the parser
    free_token(parser.current_token);
    // free_token(&parser.peek_token);
#ifdef TON_DEBUG
    fprintf(stderr, "DEBUG: Final program_exit_code: %d\n", program_exit_code);
#endif
    return program_exit_code;
}