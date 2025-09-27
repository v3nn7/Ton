#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "interpreter_stmt.h"
#include "interpreter_expr.h"
#include "environment.h"
#include "memory.h"
#include "builtin.h"
#include "error.h"

#include <string.h>
#include "lexer.h"

#include "interpreter_core.h" // for register_builtins

// Global variable to store program exit code
int program_exit_code = 0;

/**
 * Check for runtime errors and handle them appropriately
 * @param err The error to check
 * @param result The result value to release if error occurred
 * @return true if error was handled, false otherwise
 */
static bool check_and_handle_error(TonError err, Value* result) {
    if (err.code == TON_ERR_RUNTIME || err.code == TON_ERR_TYPE || err.code == TON_ERR_MEMORY || 
        err.code == TON_ERR_IMPORT || err.code == TON_ERR_INDEX || err.code == TON_ERR_EXCEPTION) {
        
        // Print detailed error information
        fprintf(stderr, "%s: %s", ton_error_code_to_string(err.code), err.message);
        if (err.filename && err.line > 0) {
            fprintf(stderr, " (at %s:%d:%d)", err.filename, err.line, err.column);
        }
        fprintf(stderr, "\n");
        
        if (result) value_release(result);
        return true;
    }
    return false;
}

/**
 * Run the interactive REPL (Read-Eval-Print Loop)
 */
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
            Value result;
            TonError err = interpret_statement(program_ast, global_env, &result);
            if (err.code == TON_OK) {
                char* str = value_to_string(&result);
                printf("%s\n", str);
                free(str);
            } else if (check_and_handle_error(err, &result)) {
                // Handled
            }
            value_release(&result);
            free_ast_node(program_ast);
        }
    }
    env_release(global_env);
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
    Value result;
    TonError err = interpret_statement(program_ast, global_env, &result);
    if (check_and_handle_error(err, &result)) {
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
        TonError err = interpret_statement((ASTNode*)main_func->body, main_env, &result);
        if (check_and_handle_error(err, &result)) {
            program_exit_code = 1;
            env_release(main_env);
            goto cleanup;
        }
        
        // Set exit code based on return
        if (err.code == TON_RETURN) {
            if (result.type == VALUE_INT) {
                program_exit_code = result.data.int_val;
            } else {
                program_exit_code = 0;
            }
        } else {
            program_exit_code = 0;
        }
        value_release(&result);

        // Free the main function's local environment
        env_release(main_env);
    } else {
        // Script mode: no main function defined; execution already happened at top level
        program_exit_code = 0;
    }

cleanup:
    // Clean up resources in proper order
    free_ast_node(program_ast);
    env_release(global_env);
    ton_free(source_code);
    if (parser.current_token) {
        free_token(parser.current_token);
    }
    if (parser.peek_token) {
        free_token(parser.peek_token);
    }
    
    // Final memory cleanup
    ton_mem_cleanup();
    
    return program_exit_code;
}