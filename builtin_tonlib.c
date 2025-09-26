#include "builtin_tonlib.h"
#include "collections.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>

// Math constants
const double TONLIB_PI = 3.14159265358979323846;
const double TONLIB_E = 2.71828182845904523536;

// Helper function to create builtin function entries
static Function* make_tonlib_builtin_fn(const char* name) {
    Function* f = (Function*)malloc(sizeof(Function));
    if (!f) return NULL;
    f->name = _strdup(name);
    f->body = NULL;
    f->closure_env = NULL;
    return f;
}

// TonLib initialization and info functions
Value* tonlib_init(Value* args, int arg_count) {
    printf("TonLib v%s by %s initialized\n", TONLIB_VERSION, TONLIB_AUTHOR);
    return &(Value){VALUE_NULL, {0}};
}

Value tonlib_version(Value* args, int arg_count) {
    return create_value_string(TONLIB_VERSION);
}

Value tonlib_author(Value* args, int arg_count) {
    return create_value_string(TONLIB_AUTHOR);
}

// Core module functions
Value tonlib_type_of(Value* args, int arg_count) {
    if (arg_count != 1) return create_value_string("unknown");
    
    Value arg = args[0];
    
    switch (arg.type) {
        case VALUE_INT: return create_value_string("int");
        case VALUE_FLOAT: return create_value_string("float");
        case VALUE_STRING: return create_value_string("string");
        case VALUE_BOOL: return create_value_string("bool");
        case VALUE_NULL: return create_value_string("null");
        case VALUE_POINTER: return create_value_string("pointer");
        case VALUE_TONLIST: return create_value_string("list");
        case VALUE_TONMAP: return create_value_string("map");
        case VALUE_TONSET: return create_value_string("set");
        default: return create_value_string("unknown");
    }
}

Value tonlib_is_null(Value* args, int arg_count) {
    if (arg_count != 1) return create_value_bool(false);
    return create_value_bool(args[0].type == VALUE_NULL);
}

Value tonlib_int_to_string(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_INT) return create_value_string("");
    
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", args[0].data.int_val);
    return create_value_string(buffer);
}

Value tonlib_float_to_string(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_FLOAT) return create_value_string("");
    
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.6f", args[0].data.float_val);
    return create_value_string(buffer);
}

Value tonlib_string_to_int(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) return create_value_int(0);
    
    return create_value_int(atoi(args[0].data.string_val));
}

Value tonlib_string_to_float(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) return create_value_float(0.0);
    
    return create_value_float(atof(args[0].data.string_val));
}

Value tonlib_bool_to_string(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_BOOL) return create_value_string("");
    
    return create_value_string(args[0].data.bool_val ? "true" : "false");
}

Value tonlib_string_to_bool(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) return create_value_bool(false);
    
    char* str = args[0].data.string_val;
    return create_value_bool(strcmp(str, "true") == 0 || strcmp(str, "1") == 0);
}

// Math module functions
Value tonlib_abs(Value* args, int arg_count) {
    if (arg_count != 1) return create_value_int(0);
    
    if (args[0].type == VALUE_INT) {
        return create_value_int(abs(args[0].data.int_val));
    } else if (args[0].type == VALUE_FLOAT) {
        return create_value_float(fabs(args[0].data.float_val));
    }
    return create_value_int(0);
}

// Collections functions - TonList
Value tonlib_list_create(Value* args, int arg_count) {
    TonList* list = tonlist_create();
    if (!list) {
        return create_value_null();
    }
    return create_value_tonlist(list);
}

Value tonlib_list_push(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_TONLIST) {
        return create_value_int(0);
    }
    
    TonList* list = (TonList*)args[0].data.tonlist_val;
    int result = tonlist_push(list, args[1]);
    return create_value_int(result);
}

Value tonlib_list_pop(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_TONLIST) {
        return create_value_null();
    }
    
    TonList* list = (TonList*)args[0].data.tonlist_val;
    return tonlist_pop(list);
}

Value tonlib_list_get(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_TONLIST || args[1].type != VALUE_INT) {
        return create_value_null();
    }
    
    TonList* list = (TonList*)args[0].data.tonlist_val;
    int index = args[1].data.int_val;
    return tonlist_get(list, index);
}

Value tonlib_list_set(Value* args, int arg_count) {
    if (arg_count != 3 || args[0].type != VALUE_TONLIST || args[1].type != VALUE_INT) {
        return create_value_int(0);
    }
    
    TonList* list = (TonList*)args[0].data.tonlist_val;
    int index = args[1].data.int_val;
    int result = tonlist_set(list, index, args[2]);
    return create_value_int(result);
}

Value tonlib_list_size(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_TONLIST) {
        return create_value_int(0);
    }
    
    TonList* list = (TonList*)args[0].data.tonlist_val;
    return create_value_int(tonlist_size(list));
}

// Collections functions - TonMap
Value tonlib_map_create(Value* args, int arg_count) {
    TonMap* map = tonmap_create();
    if (!map) {
        return create_value_null();
    }
    return create_value_tonmap(map);
}

Value tonlib_map_set(Value* args, int arg_count) {
    if (arg_count != 3 || args[0].type != VALUE_TONMAP || args[1].type != VALUE_STRING) {
        return create_value_int(0);
    }
    
    TonMap* map = (TonMap*)args[0].data.tonmap_val;
    const char* key = args[1].data.string_val;
    int result = tonmap_set(map, key, args[2]);
    return create_value_int(result);
}

Value tonlib_map_get(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_TONMAP || args[1].type != VALUE_STRING) {
        return create_value_null();
    }
    
    TonMap* map = (TonMap*)args[0].data.tonmap_val;
    const char* key = args[1].data.string_val;
    return tonmap_get(map, key);
}

Value tonlib_map_has(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_TONMAP || args[1].type != VALUE_STRING) {
        return create_value_int(0);
    }
    
    TonMap* map = (TonMap*)args[0].data.tonmap_val;
    const char* key = args[1].data.string_val;
    return create_value_int(tonmap_has(map, key));
}

Value tonlib_map_remove(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_TONMAP || args[1].type != VALUE_STRING) {
        return create_value_int(0);
    }
    
    TonMap* map = (TonMap*)args[0].data.tonmap_val;
    const char* key = args[1].data.string_val;
    return create_value_int(tonmap_remove(map, key));
}

Value tonlib_map_size(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_TONMAP) {
        return create_value_int(0);
    }
    
    TonMap* map = (TonMap*)args[0].data.tonmap_val;
    return create_value_int(tonmap_size(map));
}

// Collections functions - TonSet
Value tonlib_set_create(Value* args, int arg_count) {
    TonSet* set = tonset_create();
    if (!set) {
        return create_value_null();
    }
    return create_value_tonset(set);
}

Value tonlib_set_add(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_TONSET || args[1].type != VALUE_STRING) {
        return create_value_int(0);
    }
    
    TonSet* set = (TonSet*)args[0].data.tonset_val;
    const char* value = args[1].data.string_val;
    return create_value_int(tonset_add(set, value));
}

Value tonlib_set_has(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_TONSET || args[1].type != VALUE_STRING) {
        return create_value_int(0);
    }
    
    TonSet* set = (TonSet*)args[0].data.tonset_val;
    const char* value = args[1].data.string_val;
    return create_value_int(tonset_has(set, value));
}

Value tonlib_set_remove(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_TONSET || args[1].type != VALUE_STRING) {
        return create_value_int(0);
    }
    
    TonSet* set = (TonSet*)args[0].data.tonset_val;
    const char* value = args[1].data.string_val;
    return create_value_int(tonset_remove(set, value));
}

Value tonlib_set_size(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_TONSET) {
        return create_value_int(0);
    }
    
    TonSet* set = (TonSet*)args[0].data.tonset_val;
    return create_value_int(tonset_size(set));
}

Value tonlib_pow(Value* args, int arg_count) {
    if (arg_count != 2) return create_value_float(0.0);
    
    double base = (args[0].type == VALUE_FLOAT) ? args[0].data.float_val : (double)args[0].data.int_val;
    double exp = (args[1].type == VALUE_FLOAT) ? args[1].data.float_val : (double)args[1].data.int_val;
    
    return create_value_float(pow(base, exp));
}

Value tonlib_sqrt(Value* args, int arg_count) {
    if (arg_count != 1) return create_value_float(0.0);
    
    double val = (args[0].type == VALUE_FLOAT) ? args[0].data.float_val : (double)args[0].data.int_val;
    return create_value_float(sqrt(val));
}

Value tonlib_sin(Value* args, int arg_count) {
    if (arg_count != 1) return create_value_float(0.0);
    
    double val = (args[0].type == VALUE_FLOAT) ? args[0].data.float_val : (double)args[0].data.int_val;
    return create_value_float(sin(val));
}

Value tonlib_cos(Value* args, int arg_count) {
    if (arg_count != 1) return create_value_float(0.0);
    
    double val = (args[0].type == VALUE_FLOAT) ? args[0].data.float_val : (double)args[0].data.int_val;
    return create_value_float(cos(val));
}

Value tonlib_tan(Value* args, int arg_count) {
    if (arg_count != 1) return create_value_float(0.0);
    
    double val = (args[0].type == VALUE_FLOAT) ? args[0].data.float_val : (double)args[0].data.int_val;
    return create_value_float(tan(val));
}

Value tonlib_floor(Value* args, int arg_count) {
    if (arg_count != 1) return create_value_float(0.0);
    
    double val = (args[0].type == VALUE_FLOAT) ? args[0].data.float_val : (double)args[0].data.int_val;
    return create_value_float(floor(val));
}

Value tonlib_ceil(Value* args, int arg_count) {
    if (arg_count != 1) return create_value_float(0.0);
    
    double val = (args[0].type == VALUE_FLOAT) ? args[0].data.float_val : (double)args[0].data.int_val;
    return create_value_float(ceil(val));
}

Value tonlib_round(Value* args, int arg_count) {
    if (arg_count != 1) return create_value_float(0.0);
    
    double val = (args[0].type == VALUE_FLOAT) ? args[0].data.float_val : (double)args[0].data.int_val;
    return create_value_float(round(val));
}

Value tonlib_min(Value* args, int arg_count) {
    if (arg_count != 2) return create_value_int(0);
    
    if (args[0].type == VALUE_FLOAT || args[1].type == VALUE_FLOAT) {
        double a = (args[0].type == VALUE_FLOAT) ? args[0].data.float_val : (double)args[0].data.int_val;
        double b = (args[1].type == VALUE_FLOAT) ? args[1].data.float_val : (double)args[1].data.int_val;
        return create_value_float(fmin(a, b));
    } else {
        return create_value_int((args[0].data.int_val < args[1].data.int_val) ? args[0].data.int_val : args[1].data.int_val);
    }
}

Value tonlib_max(Value* args, int arg_count) {
    if (arg_count != 2) return create_value_int(0);
    
    if (args[0].type == VALUE_FLOAT || args[1].type == VALUE_FLOAT) {
        double a = (args[0].type == VALUE_FLOAT) ? args[0].data.float_val : (double)args[0].data.int_val;
        double b = (args[1].type == VALUE_FLOAT) ? args[1].data.float_val : (double)args[1].data.int_val;
        return create_value_float(fmax(a, b));
    } else {
        return create_value_int((args[0].data.int_val > args[1].data.int_val) ? args[0].data.int_val : args[1].data.int_val);
    }
}

Value tonlib_random(Value* args, int arg_count) {
    // Initialize random seed if not already done
    static int seed_initialized = 0;
    if (!seed_initialized) {
        srand((unsigned int)time(NULL));
        seed_initialized = 1;
    }
    
    return create_value_float((double)rand() / RAND_MAX);
}

Value tonlib_random_int(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_INT || args[1].type != VALUE_INT) {
        return create_value_int(0);
    }
    
    // Initialize random seed if not already done
    static int seed_initialized = 0;
    if (!seed_initialized) {
        srand((unsigned int)time(NULL));
        seed_initialized = 1;
    }
    
    int min = args[0].data.int_val;
    int max = args[1].data.int_val;
    
    if (min > max) {
        int temp = min;
        min = max;
        max = temp;
    }
    
    return create_value_int(min + rand() % (max - min + 1));
}

// String module functions
Value tonlib_length(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) return create_value_int(0);
    
    return create_value_int((int)strlen(args[0].data.string_val));
}

Value tonlib_concat(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_STRING || args[1].type != VALUE_STRING) {
        return create_value_string("");
    }
    
    char* str1 = args[0].data.string_val;
    char* str2 = args[1].data.string_val;
    
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char* result = malloc(len1 + len2 + 1);
    
    if (!result) return create_value_string("");
    
    strcpy(result, str1);
    strcat(result, str2);
    
    Value ret = create_value_string(result);
    free(result);
    
    return ret;
}

Value tonlib_upper(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) return create_value_string("");
    
    char* str = args[0].data.string_val;
    size_t len = strlen(str);
    char* result = malloc(len + 1);
    
    if (!result) return create_value_string("");
    
    for (size_t i = 0; i < len; i++) {
        result[i] = toupper(str[i]);
    }
    result[len] = '\0';
    
    Value ret = create_value_string(result);
    free(result);
    
    return ret;
}

Value tonlib_lower(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) return create_value_string("");
    
    char* str = args[0].data.string_val;
    size_t len = strlen(str);
    char* result = malloc(len + 1);
    
    if (!result) return create_value_string("");
    
    for (size_t i = 0; i < len; i++) {
        result[i] = tolower(str[i]);
    }
    result[len] = '\0';
    
    Value ret = create_value_string(result);
    free(result);
    
    return ret;
}

Value tonlib_trim(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) return create_value_string("");
    
    char* str = args[0].data.string_val;
    
    // Find start of non-whitespace
    while (*str && isspace(*str)) str++;
    
    if (*str == '\0') return create_value_string("");
    
    // Find end of non-whitespace
    char* end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;
    
    size_t len = end - str + 1;
    char* result = malloc(len + 1);
    
    if (!result) return create_value_string("");
    
    strncpy(result, str, len);
    result[len] = '\0';
    
    Value ret = create_value_string(result);
    free(result);
    
    return ret;
}

Value tonlib_contains(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_STRING || args[1].type != VALUE_STRING) {
        return create_value_bool(false);
    }
    
    return create_value_bool(strstr(args[0].data.string_val, args[1].data.string_val) != NULL);
}

Value tonlib_starts_with(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_STRING || args[1].type != VALUE_STRING) {
        return create_value_bool(false);
    }
    
    char* str = args[0].data.string_val;
    char* prefix = args[1].data.string_val;
    
    return create_value_bool(strncmp(str, prefix, strlen(prefix)) == 0);
}

Value tonlib_ends_with(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_STRING || args[1].type != VALUE_STRING) {
        return create_value_bool(false);
    }
    
    char* str = args[0].data.string_val;
    char* suffix = args[1].data.string_val;
    
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    
    if (suffix_len > str_len) return create_value_bool(false);
    
    return create_value_bool(strcmp(str + str_len - suffix_len, suffix) == 0);
}

Value tonlib_is_empty(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) return create_value_bool(true);
    
    return create_value_bool(strlen(args[0].data.string_val) == 0);
}

// Memory management functions
Value tonlib_alloc(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_INT) {
        return create_value_null();
    }
    
    size_t size = (size_t)args[0].data.int_val;
    if (size <= 0) {
        return create_value_null();
    }
    
    void* ptr = malloc(size);
    if (!ptr) {
        return create_value_null();
    }
    
    return create_value_pointer(ptr);
}

Value tonlib_free(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_POINTER) {
        return create_value_null();
    }
    
    void* ptr = args[0].data.pointer_val;
    if (ptr) {
        free(ptr);
    }
    
    return create_value_int(0); // Zwracam int zamiast NULL
}

// Placeholder implementations for more complex functions
Value tonlib_substring(Value* args, int arg_count) {
    if (arg_count < 2 || arg_count > 3 || args[0].type != VALUE_STRING || args[1].type != VALUE_INT) {
        return create_value_string("");
    }
    
    char* str = args[0].data.string_val;
    int start = args[1].data.int_val;
    int len = strlen(str);
    
    if (start < 0 || start >= len) {
        return create_value_string("");
    }
    
    int end = len;
    if (arg_count == 3 && args[2].type == VALUE_INT) {
        end = args[2].data.int_val;
        if (end > len) end = len;
        if (end <= start) return create_value_string("");
    }
    
    int substr_len = end - start;
    char* result = malloc(substr_len + 1);
    strncpy(result, str + start, substr_len);
    result[substr_len] = '\0';
    
    Value ret = create_value_string(result);
    free(result);
    return ret;
}

Value tonlib_index_of(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_STRING || args[1].type != VALUE_STRING) {
        return create_value_int(-1);
    }
    
    char* haystack = args[0].data.string_val;
    char* needle = args[1].data.string_val;
    
    char* found = strstr(haystack, needle);
    if (found) {
        return create_value_int((int)(found - haystack));
    }
    return create_value_int(-1);
}

Value tonlib_replace(Value* args, int arg_count) {
    if (arg_count != 3 || args[0].type != VALUE_STRING || 
        args[1].type != VALUE_STRING || args[2].type != VALUE_STRING) {
        return create_value_string("");
    }
    
    char* str = args[0].data.string_val;
    char* old_substr = args[1].data.string_val;
    char* new_substr = args[2].data.string_val;
    
    char* found = strstr(str, old_substr);
    if (!found) {
        return create_value_string(str);
    }
    
    int old_len = strlen(old_substr);
    int new_len = strlen(new_substr);
    int str_len = strlen(str);
    int result_len = str_len - old_len + new_len;
    
    char* result = malloc(result_len + 1);
    int pos = found - str;
    
    strncpy(result, str, pos);
    strcpy(result + pos, new_substr);
    strcpy(result + pos + new_len, str + pos + old_len);
    
    Value ret = create_value_string(result);
    free(result);
    return ret;
}

Value tonlib_split(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_STRING || args[1].type != VALUE_STRING) {
        return create_value_null();
    }
    
    char* str = args[0].data.string_val;
    char* delimiter = args[1].data.string_val;
    
    if (strlen(str) == 0) {
        return create_value_null();
    }
    
    // Count occurrences of delimiter to determine array size
    int count = 1;
    char* temp = str;
    while ((temp = strstr(temp, delimiter)) != NULL) {
        count++;
        temp += strlen(delimiter);
    }
    
    // Create a simple string representation of the array for now
    // In a full implementation, this would return an actual array type
    char* result = malloc(strlen(str) + count * 10); // Rough estimate
    strcpy(result, "[");
    
    char* str_copy = malloc(strlen(str) + 1);
    strcpy(str_copy, str);
    
    char* token = strtok(str_copy, delimiter);
    int first = 1;
    while (token != NULL) {
        if (!first) {
            strcat(result, ", ");
        }
        strcat(result, "\"");
        strcat(result, token);
        strcat(result, "\"");
        first = 0;
        token = strtok(NULL, delimiter);
    }
    strcat(result, "]");
    
    free(str_copy);
    Value ret = create_value_string(result);
    free(result);
    return ret;
}

Value tonlib_join(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_STRING || args[1].type != VALUE_STRING) {
        return create_value_string("");
    }
    
    char* array_str = args[0].data.string_val;
    char* delimiter = args[1].data.string_val;
    
    // Simple implementation: parse array-like string "[\"a\", \"b\", \"c\"]"
    // and join elements with delimiter
    
    if (strlen(array_str) < 2 || array_str[0] != '[' || array_str[strlen(array_str)-1] != ']') {
        return create_value_string("");
    }
    
    // Remove brackets
    char* content = malloc(strlen(array_str) - 1);
    strncpy(content, array_str + 1, strlen(array_str) - 2);
    content[strlen(array_str) - 2] = '\0';
    
    // Simple parsing - split by ", " and remove quotes
    char* result = malloc(strlen(content) + strlen(delimiter) * 10);
    result[0] = '\0';
    
    char* token = strtok(content, ",");
    int first = 1;
    
    while (token != NULL) {
        // Trim whitespace
        while (*token == ' ') token++;
        
        // Remove quotes if present
        if (token[0] == '"' && token[strlen(token)-1] == '"') {
            token[strlen(token)-1] = '\0';
            token++;
        }
        
        if (!first) {
            strcat(result, delimiter);
        }
        strcat(result, token);
        first = 0;
        
        token = strtok(NULL, ",");
    }
    
    free(content);
    Value ret = create_value_string(result);
    free(result);
    return ret;
}

// Install all TonLib built-in functions
void install_tonlib_builtins(Environment* env) {
    if (!env) return;

    // TonLib info functions
    env_add_function(env, "tonlib_init", make_tonlib_builtin_fn("tonlib_init"));
    env_add_function(env, "tonlib_version", make_tonlib_builtin_fn("tonlib_version"));
    env_add_function(env, "tonlib_author", make_tonlib_builtin_fn("tonlib_author"));
    env_add_function(env, "tonlib_info", make_tonlib_builtin_fn("tonlib_info"));

    // Core module functions
    env_add_function(env, "type_of", make_tonlib_builtin_fn("type_of"));
    env_add_function(env, "is_null", make_tonlib_builtin_fn("is_null"));
    env_add_function(env, "int_to_string", make_tonlib_builtin_fn("int_to_string"));
    env_add_function(env, "float_to_string", make_tonlib_builtin_fn("float_to_string"));
    env_add_function(env, "string_to_int", make_tonlib_builtin_fn("string_to_int"));
    env_add_function(env, "string_to_float", make_tonlib_builtin_fn("string_to_float"));
    env_add_function(env, "bool_to_string", make_tonlib_builtin_fn("bool_to_string"));
    env_add_function(env, "string_to_bool", make_tonlib_builtin_fn("string_to_bool"));

    // Math module functions
    env_add_function(env, "abs", make_tonlib_builtin_fn("abs"));
    env_add_function(env, "pow", make_tonlib_builtin_fn("pow"));
    env_add_function(env, "sqrt", make_tonlib_builtin_fn("sqrt"));
    env_add_function(env, "sin", make_tonlib_builtin_fn("sin"));
    env_add_function(env, "cos", make_tonlib_builtin_fn("cos"));
    env_add_function(env, "tan", make_tonlib_builtin_fn("tan"));
    env_add_function(env, "floor", make_tonlib_builtin_fn("floor"));
    env_add_function(env, "ceil", make_tonlib_builtin_fn("ceil"));
    env_add_function(env, "round", make_tonlib_builtin_fn("round"));
    env_add_function(env, "min", make_tonlib_builtin_fn("min"));
    env_add_function(env, "max", make_tonlib_builtin_fn("max"));
    env_add_function(env, "random", make_tonlib_builtin_fn("random"));
    env_add_function(env, "random_int", make_tonlib_builtin_fn("random_int"));

    // String module functions
    env_add_function(env, "length", make_tonlib_builtin_fn("length"));
    env_add_function(env, "concat", make_tonlib_builtin_fn("concat"));
    env_add_function(env, "substring", make_tonlib_builtin_fn("substring"));
    env_add_function(env, "index_of", make_tonlib_builtin_fn("index_of"));
    env_add_function(env, "to_upper", make_tonlib_builtin_fn("to_upper"));
    env_add_function(env, "to_lower", make_tonlib_builtin_fn("to_lower"));
    env_add_function(env, "trim", make_tonlib_builtin_fn("trim"));
    env_add_function(env, "contains", make_tonlib_builtin_fn("contains"));
    env_add_function(env, "starts_with", make_tonlib_builtin_fn("starts_with"));
    env_add_function(env, "ends_with", make_tonlib_builtin_fn("ends_with"));
    env_add_function(env, "replace", make_tonlib_builtin_fn("replace"));
    env_add_function(env, "split", make_tonlib_builtin_fn("split"));
    env_add_function(env, "join", make_tonlib_builtin_fn("join"));
    env_add_function(env, "is_empty", make_tonlib_builtin_fn("is_empty"));

    // Memory management functions
    env_add_function(env, "alloc", make_tonlib_builtin_fn("alloc"));
    env_add_function(env, "free", make_tonlib_builtin_fn("free"));
    
    // Add math constants as variables
    env_add_variable(env, "TONLIB_PI", create_value_float(TONLIB_PI));
    env_add_variable(env, "TONLIB_E", create_value_float(TONLIB_E));
}

// Function dispatcher for TonLib functions
Value call_tonlib_function(const char* function_name, ASTNode** arguments, int arg_count, Environment* env) {
    // Convert AST arguments to Value array
    Value* args = NULL;
    if (arg_count > 0) {
        args = (Value*)malloc(sizeof(Value) * arg_count);
        if (!args) {
            return create_value_null();
        }
        
        for (int i = 0; i < arg_count; i++) {
            args[i] = interpret_ast(arguments[i], env);
        }
    }
    
    Value result = create_value_null();
    
    // TonLib info functions
    if (strcmp(function_name, "tonlib_version") == 0) {
        result = tonlib_version(args, arg_count);
    } else if (strcmp(function_name, "tonlib_author") == 0) {
        result = tonlib_author(args, arg_count);
    }
    // Core module functions
    else if (strcmp(function_name, "type_of") == 0) {
        result = tonlib_type_of(args, arg_count);
    } else if (strcmp(function_name, "is_null") == 0) {
        result = tonlib_is_null(args, arg_count);
    } else if (strcmp(function_name, "int_to_string") == 0) {
        result = tonlib_int_to_string(args, arg_count);
    } else if (strcmp(function_name, "float_to_string") == 0) {
        result = tonlib_float_to_string(args, arg_count);
    } else if (strcmp(function_name, "string_to_int") == 0) {
        result = tonlib_string_to_int(args, arg_count);
    } else if (strcmp(function_name, "string_to_float") == 0) {
        result = tonlib_string_to_float(args, arg_count);
    } else if (strcmp(function_name, "bool_to_string") == 0) {
        result = tonlib_bool_to_string(args, arg_count);
    } else if (strcmp(function_name, "string_to_bool") == 0) {
        result = tonlib_string_to_bool(args, arg_count);
    }
    // Math module functions
    else if (strcmp(function_name, "abs") == 0) {
        result = tonlib_abs(args, arg_count);
    } else if (strcmp(function_name, "pow") == 0) {
        result = tonlib_pow(args, arg_count);
    } else if (strcmp(function_name, "sqrt") == 0) {
        result = tonlib_sqrt(args, arg_count);
    } else if (strcmp(function_name, "sin") == 0) {
        result = tonlib_sin(args, arg_count);
    } else if (strcmp(function_name, "cos") == 0) {
        result = tonlib_cos(args, arg_count);
    } else if (strcmp(function_name, "tan") == 0) {
        result = tonlib_tan(args, arg_count);
    } else if (strcmp(function_name, "floor") == 0) {
        result = tonlib_floor(args, arg_count);
    } else if (strcmp(function_name, "ceil") == 0) {
        result = tonlib_ceil(args, arg_count);
    } else if (strcmp(function_name, "round") == 0) {
        result = tonlib_round(args, arg_count);
    } else if (strcmp(function_name, "min") == 0) {
        result = tonlib_min(args, arg_count);
    } else if (strcmp(function_name, "max") == 0) {
        result = tonlib_max(args, arg_count);
    } else if (strcmp(function_name, "random") == 0) {
        result = tonlib_random(args, arg_count);
    } else if (strcmp(function_name, "random_int") == 0) {
        result = tonlib_random_int(args, arg_count);
    }
    // String module functions
    else if (strcmp(function_name, "length") == 0) {
        result = tonlib_length(args, arg_count);
    } else if (strcmp(function_name, "concat") == 0) {
        result = tonlib_concat(args, arg_count);
    } else if (strcmp(function_name, "to_upper") == 0) {
        result = tonlib_upper(args, arg_count);
    } else if (strcmp(function_name, "to_lower") == 0) {
        result = tonlib_lower(args, arg_count);
    } else if (strcmp(function_name, "trim") == 0) {
        result = tonlib_trim(args, arg_count);
    } else if (strcmp(function_name, "contains") == 0) {
        result = tonlib_contains(args, arg_count);
    } else if (strcmp(function_name, "starts_with") == 0) {
        result = tonlib_starts_with(args, arg_count);
    } else if (strcmp(function_name, "ends_with") == 0) {
        result = tonlib_ends_with(args, arg_count);
    } else if (strcmp(function_name, "is_empty") == 0) {
        result = tonlib_is_empty(args, arg_count);
    } else if (strcmp(function_name, "substring") == 0) {
        result = tonlib_substring(args, arg_count);
    } else if (strcmp(function_name, "index_of") == 0) {
        result = tonlib_index_of(args, arg_count);
    } else if (strcmp(function_name, "split") == 0) {
        result = tonlib_split(args, arg_count);
    }
    // Memory management functions
    else if (strcmp(function_name, "alloc") == 0) {
        result = tonlib_alloc(args, arg_count);
    } else if (strcmp(function_name, "free") == 0) {
        result = tonlib_free(args, arg_count);
    }
    // Collections functions - TonList
    else if (strcmp(function_name, "list_create") == 0) {
        result = tonlib_list_create(args, arg_count);
    } else if (strcmp(function_name, "list_push") == 0) {
        result = tonlib_list_push(args, arg_count);
    } else if (strcmp(function_name, "list_pop") == 0) {
        result = tonlib_list_pop(args, arg_count);
    } else if (strcmp(function_name, "list_get") == 0) {
        result = tonlib_list_get(args, arg_count);
    } else if (strcmp(function_name, "list_set") == 0) {
        result = tonlib_list_set(args, arg_count);
    } else if (strcmp(function_name, "list_size") == 0) {
        result = tonlib_list_size(args, arg_count);
    }
    // Collections functions - TonMap
    else if (strcmp(function_name, "map_create") == 0) {
        result = tonlib_map_create(args, arg_count);
    } else if (strcmp(function_name, "map_set") == 0) {
        result = tonlib_map_set(args, arg_count);
    } else if (strcmp(function_name, "map_get") == 0) {
        result = tonlib_map_get(args, arg_count);
    } else if (strcmp(function_name, "map_has") == 0) {
        result = tonlib_map_has(args, arg_count);
    } else if (strcmp(function_name, "map_remove") == 0) {
        result = tonlib_map_remove(args, arg_count);
    } else if (strcmp(function_name, "map_size") == 0) {
        result = tonlib_map_size(args, arg_count);
    }
    // Collections functions - TonSet
    else if (strcmp(function_name, "set_create") == 0) {
        result = tonlib_set_create(args, arg_count);
    } else if (strcmp(function_name, "set_add") == 0) {
        result = tonlib_set_add(args, arg_count);
    } else if (strcmp(function_name, "set_has") == 0) {
        result = tonlib_set_has(args, arg_count);
    } else if (strcmp(function_name, "set_remove") == 0) {
        result = tonlib_set_remove(args, arg_count);
    } else if (strcmp(function_name, "set_size") == 0) {
        result = tonlib_set_size(args, arg_count);
    }
    // File I/O functions
    else if (strcmp(function_name, "file_read_all") == 0) {
        result = tonlib_file_read_all(args, arg_count);
    } else if (strcmp(function_name, "file_write_all") == 0) {
        result = tonlib_file_write_all(args, arg_count);
    }
    
    // Clean up arguments
    if (args) {
        free(args);
    }
    
    return result;
}

// File I/O functions
Value tonlib_file_read_all(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) {
        return create_value_string("");
    }
    
    const char* filename = args[0].data.string_val;
    FILE* file = fopen(filename, "r");
    if (!file) {
        return create_value_string("");
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Allocate buffer
    char* buffer = (char*)malloc(file_size + 1);
    if (!buffer) {
        fclose(file);
        return create_value_string("");
    }
    
    // Read file content
    size_t bytes_read = fread(buffer, 1, file_size, file);
    buffer[bytes_read] = '\0';
    
    fclose(file);
    
    Value result = create_value_string(buffer);
    free(buffer);
    return result;
}

Value tonlib_file_write_all(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_STRING || args[1].type != VALUE_STRING) {
        return create_value_bool(0);
    }
    
    const char* filename = args[0].data.string_val;
    const char* content = args[1].data.string_val;
    
    FILE* file = fopen(filename, "w");
    if (!file) {
        return create_value_bool(0);
    }
    
    size_t content_len = strlen(content);
    size_t bytes_written = fwrite(content, 1, content_len, file);
    fclose(file);
    
    return create_value_bool(bytes_written == content_len);
}