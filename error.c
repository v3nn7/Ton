#include "error.h"
#include <stddef.h>

struct TonError ton_ok() { struct TonError e = { TON_OK, NULL }; return e; }
struct TonError ton_error(TonErrorCode code, const char* msg) { struct TonError e = { code, msg }; return e; }