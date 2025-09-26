#ifndef TON_ARRAY_H
#define TON_ARRAY_H

#include <stddef.h>
#include "interpreter.h"

typedef enum { ARRAY_STATIC, ARRAY_DYNAMIC } ArrayKind;

typedef struct TonArray {
    ArrayKind kind;
    size_t length;
    size_t capacity;
    Value* elements;
    const char* element_type_name;
} TonArray;

TonArray* create_static_array(size_t length);
TonArray* create_dynamic_array(size_t initial_capacity);
void      destroy_array(TonArray* arr);

int       array_push(TonArray* arr, Value v);
Value     array_pop(TonArray* arr);
Value     array_get(const TonArray* arr, size_t index);
int       array_set(TonArray* arr, size_t index, Value v);

#endif // TON_ARRAY_H