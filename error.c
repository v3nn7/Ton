#include "error.h"
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

struct TonError ton_ok() { struct TonError e = { TON_OK, NULL, 0, 0, NULL }; return e; }
struct TonError ton_error(TonErrorCode code, const char* message, int line, int column, const char* filename) {
    struct TonError e = { code, message, line, column, filename };
    return e;
}

void runtime_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, "Runtime Error: ", args);
    va_end(args);
    fprintf(stderr, "\n");
}