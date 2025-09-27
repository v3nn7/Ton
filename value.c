#include "value.h"
#include "memory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"
#include "environment.h"
#include "interpreter.h" // Add for USER_DEFINED

Value create_value_int(int i) {
    Value val;
    val.type = VALUE_INT;
    val.data.int_val = i;
    val.ref_count = 0;
    return val;
}

Value create_value_fn(Function* fn) {
    Value val;
    val.type = VALUE_FN;
    val.data.function_value = fn;
    val.ref_count = 1;
    return val;
}

Value create_value_string(const char* s) {
    Value val;
    val.type = VALUE_STRING;
    val.data.string_val = ton_strdup(s);
    val.ref_count = 1;
    return val;
}

Value create_value_float(double f) {
    Value val;
    val.type = VALUE_FLOAT;
    val.data.float_val = f;
    val.ref_count = 0;
    return val;
}

Value create_value_bool(int b) {
    Value val;
    val.type = VALUE_BOOL;
    val.data.bool_val = b;
    val.ref_count = 0;
    return val;
}

Value create_value_null(void) {
    Value val;
    val.type = VALUE_NULL;
    val.ref_count = 0;
    return val;
}

Value create_value_pointer(void* ptr) {
    Value val;
    val.type = VALUE_POINTER;
    val.data.pointer_val = ptr;
    val.ref_count = 0;
    return val;
}

Value create_value_array(void* arr) {
    Value val;
    val.type = VALUE_ARRAY;
    val.data.array_val = arr;
    val.ref_count = 1;
    return val;
}

Value create_value_tonlist(void* list) {
    Value val;
    val.type = VALUE_TONLIST;
    val.data.tonlist_val = list;
    val.ref_count = 1;
    return val;
}

Value create_value_tonmap(void* map) {
    Value val;
    val.type = VALUE_TONMAP;
    val.data.tonmap_val = map;
    val.ref_count = 1;
    return val;
}

Value create_value_tonset(void* set) {
    Value val;
    val.type = VALUE_TONSET;
    val.data.tonset_val = set;
    val.ref_count = 1;
    return val;
}

Value create_value_method(Value* object, char* method_name) {
    Value val;
    val.type = VALUE_METHOD;
    val.data.method_val.object = object;
    val.data.method_val.method_name = ton_strdup(method_name);
    val.ref_count = 1;
    return val;
}

Value create_value_char(char c) {
    Value val;
    val.type = VALUE_CHAR;
    val.data.char_val = c;
    val.ref_count = 0;
    return val;
}

Value create_value_error(const char* message)
{
    Value v;
    v.type = VALUE_ERROR;
    v.ref_count = 0;
    v.data.error_message = ton_strdup(message);
    return v;
}

void value_add_ref(Value* val) {
    if (val->type == VALUE_STRING || val->type == VALUE_FN || val->type == VALUE_ARRAY || val->type == VALUE_TONLIST || val->type == VALUE_TONMAP || val->type == VALUE_TONSET || val->type == VALUE_METHOD) {
        val->ref_count++;
    }
}

void value_release(Value* val) {
    if (val->type == VALUE_STRING || val->type == VALUE_FN || val->type == VALUE_ARRAY || val->type == VALUE_TONLIST || val->type == VALUE_TONMAP || val->type == VALUE_TONSET || val->type == VALUE_METHOD || val->type == VALUE_ERROR) {
        val->ref_count--;
        if (val->ref_count == 0) {
            if (val->type == VALUE_STRING) {
                ton_free(val->data.string_val);
            } else if (val->type == VALUE_METHOD) {
                ton_free(val->data.method_val.method_name);
            } else if (val->type == VALUE_ERROR) {
                ton_free(val->data.error_message);
            } else if (val->type == VALUE_FN) {
                if (val->data.function_value->type == USER_DEFINED) {
                    env_release(val->data.function_value->closure_env);
                }
                ton_free(val->data.function_value->name);
                ton_free(val->data.function_value);
            }
        }
    }
}

char* value_to_string(Value* val) {
    char* str = (char*)ton_malloc(256);
    if (!str) {
        return NULL;
    }

    switch (val->type) {
        case VALUE_INT:
            sprintf(str, "%d", val->data.int_val);
            break;
        case VALUE_FLOAT:
            sprintf(str, "%f", val->data.float_val);
            break;
        case VALUE_BOOL:
            sprintf(str, "%s", val->data.bool_val ? "true" : "false");
            break;
        case VALUE_CHAR:
            sprintf(str, "'%c'", val->data.char_val);
            break;
        case VALUE_STRING:
            ton_free(str);
            return ton_strdup(val->data.string_val);
        case VALUE_NULL:
            strcpy(str, "null");
            break;
        case VALUE_POINTER:
            sprintf(str, "pointer@%p", val->data.pointer_val);
            break;
        case VALUE_ARRAY:
            strcpy(str, "[array]");
            break;
        case VALUE_TONLIST:
            strcpy(str, "[tonlist]");
            break;
        case VALUE_TONMAP:
            strcpy(str, "[tonmap]");
            break;
        case VALUE_TONSET:
            strcpy(str, "[tonset]");
            break;
        case VALUE_ERROR:
            sprintf(str, "error: %s", val->data.error_message);
            break;
        default:
            strcpy(str, "[unknown]");
            break;
    }
    return str;
}

const char* value_type_to_string(ValueType type) {
    switch (type) {
        case VALUE_INT: return "int";
        case VALUE_FN: return "function";
        case VALUE_STRING: return "string";
        case VALUE_FLOAT: return "float";
        case VALUE_BOOL: return "bool";
        case VALUE_NULL: return "null";
        case VALUE_POINTER: return "pointer";
        case VALUE_ARRAY: return "array";
        case VALUE_TONLIST: return "tonlist";
        case VALUE_TONMAP: return "tonmap";
        case VALUE_TONSET: return "tonset";
        case VALUE_METHOD: return "method";
        case VALUE_CHAR: return "char";
        case VALUE_ERROR: return "error";
        default: return "unknown";
    }
}

bool compare_values(Value* a, Value* b) {
    if (a->type != b->type) {
        return false;
    }

    switch (a->type) {
        case VALUE_INT:
            return a->data.int_val == b->data.int_val;
        case VALUE_FLOAT:
            return a->data.float_val == b->data.float_val;
        case VALUE_BOOL:
            return a->data.bool_val == b->data.bool_val;
        case VALUE_CHAR:
            return a->data.char_val == b->data.char_val;
        case VALUE_STRING:
            return strcmp(a->data.string_val, b->data.string_val) == 0;
        case VALUE_NULL:
            return true; // null is always equal to null
        default:
            return false; // Other types are not comparable
    }
}