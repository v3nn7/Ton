#ifndef TON_ERROR_H
#define TON_ERROR_H

typedef enum {
    TON_OK = 0,
    TON_RETURN,
    TON_BREAK,
    TON_CONTINUE,
    TON_ERR_RUNTIME,
    TON_ERR_TYPE,
    TON_ERR_MEMORY,
    TON_ERR_IMPORT,
    TON_ERR_INDEX,
    TON_ERR_SYNTAX // Add this line
} TonErrorCode;

typedef struct TonError {
    TonErrorCode code;
    const char* message;
    int line;
    int column;
    const char* filename;
} TonError;

struct TonError ton_ok();
struct TonError ton_error(TonErrorCode code, const char* msg, int line, int column, const char* filename);

void runtime_error(const char* fmt, ...);

#endif // TON_ERROR_H