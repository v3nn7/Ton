#ifndef TON_ERROR_H
#define TON_ERROR_H

/**
 * Error codes used throughout the Ton interpreter
 */
typedef enum {
    TON_OK = 0,          // Success
    TON_RETURN,          // Function return
    TON_BREAK,           // Loop break
    TON_CONTINUE,        // Loop continue
    TON_ERR_RUNTIME,     // Runtime error
    TON_ERR_TYPE,        // Type mismatch error
    TON_ERR_MEMORY,      // Memory allocation error
    TON_ERR_IMPORT,      // Import/module error
    TON_ERR_INDEX,       // Array/string index error
    TON_ERR_SYNTAX,      // Syntax error
    TON_ERR_EXCEPTION    // Exception handling error
} TonErrorCode;

/**
 * Error structure containing detailed error information
 */
typedef struct TonError {
    TonErrorCode code;      // Error code
    const char* message;    // Error message
    int line;              // Line number where error occurred
    int column;            // Column number where error occurred
    const char* filename;   // File name where error occurred
} TonError;

// Function declarations
struct TonError ton_ok();
struct TonError ton_error(TonErrorCode code, const char* msg, int line, int column, const char* filename);
void runtime_error(const char* fmt, ...);
const char* ton_error_code_to_string(TonErrorCode code);
int ton_error_is_error(TonError error);

#endif // TON_ERROR_H