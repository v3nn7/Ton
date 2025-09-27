#include "error.h"
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/**
 * Create a success error structure
 * @return TonError structure indicating success
 */
struct TonError ton_ok() { 
    struct TonError e = { TON_OK, NULL, 0, 0, NULL }; 
    return e; 
}

/**
 * Create an error structure with detailed information
 * @param code Error code
 * @param message Error message
 * @param line Line number where error occurred
 * @param column Column number where error occurred
 * @param filename File name where error occurred
 * @return TonError structure with error details
 */
struct TonError ton_error(TonErrorCode code, const char* message, int line, int column, const char* filename) {
    struct TonError e = { code, message, line, column, filename };
    return e;
}

/**
 * Check if a TonError represents an error condition
 * @param error The error to check
 * @return 1 if error, 0 if success
 */
int ton_error_is_error(TonError error) {
    return error.code != TON_OK;
}

/**
 * Print a formatted runtime error message to stderr
 * @param fmt Format string
 * @param ... Variable arguments for formatting
 */
void runtime_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "Runtime Error: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

/**
 * Get a human-readable description of an error code
 * @param code The error code
 * @return String description of the error
 */
const char* ton_error_code_to_string(TonErrorCode code) {
    switch (code) {
        case TON_OK: return "Success";
        case TON_ERR_SYNTAX: return "Syntax Error";
        case TON_ERR_RUNTIME: return "Runtime Error";
        case TON_ERR_TYPE: return "Type Error";
        case TON_ERR_MEMORY: return "Memory Error";
        case TON_ERR_IMPORT: return "Import Error";
        case TON_ERR_INDEX: return "Index Error";
        case TON_ERR_EXCEPTION: return "Exception";
        case TON_RETURN: return "Return";
        case TON_BREAK: return "Break";
        case TON_CONTINUE: return "Continue";
        default: return "Unknown Error";
    }
}