#include "array.h"
#include <stdlib.h>
#include <string.h>

TonArray* create_static_array(size_t length) {
    TonArray* a = (TonArray*)malloc(sizeof(TonArray));
    if (!a) return NULL;
    a->kind = ARRAY_STATIC;
    a->length = length;
    a->capacity = length;
    a->elements = (Value*)calloc(length, sizeof(Value));
    a->element_type_name = NULL;
    return a;
}

TonArray* create_dynamic_array(size_t initial_capacity) {
    TonArray* a = (TonArray*)malloc(sizeof(TonArray));
    if (!a) return NULL;
    a->kind = ARRAY_DYNAMIC;
    a->length = 0;
    a->capacity = initial_capacity > 0 ? initial_capacity : 4;
    a->elements = (Value*)calloc(a->capacity, sizeof(Value));
    a->element_type_name = NULL;
    return a;
}

void destroy_array(TonArray* arr) {
    if (!arr) return;
    if (arr->elements) {
        // Note: currently Value has no value_free here; we assume plain values
        free(arr->elements);
    }
    free(arr);
}

int array_push(TonArray* arr, Value v) {
    if (!arr || arr->kind != ARRAY_DYNAMIC) return 0;
    if (arr->length >= arr->capacity) {
        size_t new_cap = arr->capacity * 2;
        Value* t = (Value*)realloc(arr->elements, new_cap * sizeof(Value));
        if (!t) return 0;
        arr->elements = t; arr->capacity = new_cap;
    }
    arr->elements[arr->length++] = v;
    return 1;
}

Value array_pop(TonArray* arr) {
    Value out; out.type = VALUE_INT; out.data.int_val = 0;
    if (!arr || arr->kind != ARRAY_DYNAMIC || arr->length == 0) return out;
    return arr->elements[--arr->length];
}

Value array_get(const TonArray* arr, size_t index) {
    Value out; out.type = VALUE_INT; out.data.int_val = 0;
    if (!arr || index >= arr->length) return out;
    return arr->elements[index];
}

int array_set(TonArray* arr, size_t index, Value v) {
    if (!arr || index >= arr->length) return 0;
    arr->elements[index] = v;
    return 1;
}