#include "error.h"
#include <stddef.h>

struct TonError ton_ok() { struct TonError e = { TON_OK, NULL }; return e; }
struct TonError ton_error(TonErrorCode code, const char* msg) { struct TonError e = { code, msg }; return e; }

#include <stdarg.h>
#include <stdio.h>

void runtime_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}