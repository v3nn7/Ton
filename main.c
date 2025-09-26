#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "interpreter_stmt.h"
#include "interpreter_expr.h"
#include "environment.h"
#include "memory.h"
#include "builtin.h"

#include <string.h>
#include "lexer.h"

#include "interpreter_core.h" // for register_builtins

int program_exit_code = 0;

// Function to check for runtime errors and handle them
static int check_and_handle_error(Value val) {
    if (val.type == VALUE_ERROR) {
        fprintf(stderr, "Runtime Error: %s\n", val.data.error_message);
        value_release(&val);
        return 1; // Indicates an error occurred
    }
    return 0; // No error
}

void run_repl() {
    char line[1024];
    Environment* global_env = create_environment();
    install_builtins(global_env);

    printf("Ton REPL\n");
    while (1) {
        printf(">> ");
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        Lexer lexer;
        init_lexer(&lexer, line);

        Parser parser;
        init_parser(&parser, &lexer);

        ASTNode* program_ast = parse_program(&parser);
        if (program_ast) {
            Value result = interpret_statement(program_ast, global_env);
            if (result.type != VALUE_ERROR) {
                char* result_str = value_to_string(&result);
                printf("%s\n", result_str);
                free(result_str);
            }
            value_release(&result);
            free_ast_node(program_ast);
        }
    }
    destroy_environment(global_env);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        run_repl();
        return 0;
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

    char* source_code = (char*)ton_malloc(file_size + 1);
    if (source_code == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(file);
        return 1;
    }

    size_t bytes_read = fread(source_code, 1, file_size, file);
    if (bytes_read != (size_t)file_size) {
        fprintf(stderr, "Error reading file \"%s\".\n", filename);
        ton_free(source_code);
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
    install_builtins(global_env);


    // Interpret the AST (top-level statements)
    Value result = interpret_statement(program_ast, global_env);
    if (check_and_handle_error(result)) {
        program_exit_code = 1;
        goto cleanup;
    }
    value_release(&result);


    // After interpreting the entire AST, find and optionally execute the 'main' function
    Function* main_func = env_get_function(global_env, "main");
    if (main_func != NULL) {
        // Create a new environment for the main function's local scope
        Environment* main_env = create_child_environment(global_env);

        // Interpret the body of the main function
        // Execute the program
        result = interpret_statement((ASTNode*)main_func->body, main_env);
        if (check_and_handle_error(result)) {
            program_exit_code = 1;
            destroy_environment(main_env);
            goto cleanup;
        }
        
        // Set exit code based on result
        if (result.type == VALUE_INT) {
            program_exit_code = result.data.int_val;
        } else {
            // If main returns non-int or has no explicit return, default to 0
            program_exit_code = 0;
        }
        value_release(&result);

        // Free the main function's local environment
        destroy_environment(main_env);
    } else {
        // Script mode: no main function defined; execution already happened at top level
        program_exit_code = 0;
    }

cleanup:
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
    ton_free(source_code);
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