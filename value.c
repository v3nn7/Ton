#include "value.h"
#include <stdlib.h>
#include <string.h>

// Local strdup replacement used by create_value_string
static char* my_strdup_local2(const char* s) {
    if (s == NULL) return NULL;
    size_t len = strlen(s) + 1;
    char* d = (char*)malloc(len);
    if (!d) return NULL;
    memcpy(d, s, len);
    return d;
}

Value create_value_int(int i) {
    Value val;
    val.type = VALUE_INT;
    val.data.int_val = i;
    return val;
}

Value create_value_fn(Function* fn) {
    Value val;
    val.type = VALUE_FN;
    val.data.function_value = fn;
    return val;
}

Value create_value_string(char* s) {
    Value val;
    val.type = VALUE_STRING;
    val.data.string_val = my_strdup_local2(s); // Duplicate the string to manage its memory
    return val;
}

Value create_value_float(double f) {
    Value val;
    val.type = VALUE_FLOAT;
    val.data.float_val = f;
    return val;
}

Value create_value_bool(int b) {
    Value val;
    val.type = VALUE_BOOL;
    val.data.bool_val = b;
    return val;
}

Value create_value_null(void) {
    Value val;
    val.type = VALUE_NULL;
    // No data needed for null values
    return val;
}

Value create_value_pointer(void* ptr) {
    Value val;
    val.type = VALUE_POINTER;
    val.data.pointer_val = ptr;
    return val;
}

Value create_value_array(void* arr) {
    Value val;
    val.type = VALUE_ARRAY;
    val.data.array_val = arr;
    return val;
}

Value create_value_tonlist(void* list) {
    Value val;
    val.type = VALUE_TONLIST;
    val.data.tonlist_val = list;
    return val;
}

Value create_value_tonmap(void* map) {
    Value val;
    val.type = VALUE_TONMAP;
    val.data.tonmap_val = map;
    return val;
}

Value create_value_tonset(void* set) {
    Value val;
    val.type = VALUE_TONSET;
    val.data.tonset_val = set;
    return val;
}