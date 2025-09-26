#ifndef TON_ERROR_H
#define TON_ERROR_H

typedef enum {
    TON_OK = 0,
    TON_ERR_RUNTIME,
    TON_ERR_TYPE,
    TON_ERR_MEMORY,
    TON_ERR_IMPORT,
    TON_ERR_INDEX
} TonErrorCode;

typedef struct TonError {
    TonErrorCode code;
    const char* message;
} TonError;

struct TonError ton_ok();
struct TonError ton_error(TonErrorCode code, const char* msg);

#endif // TON_ERROR_H