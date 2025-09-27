#include "builtin_tonlib.h"
#include "builtin.h"
#include "collections.h"
#include "sha256.h"
#include "md5.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>
#include "array.h" // Added to resolve TonArray and tonarray_create errors
#include "memory.h" // Added to resolve my_strdup undefined reference

// Helper function to create builtin function stubs
static Function* make_builtin_fn(const char* name) {
    Function* f = (Function*)ton_malloc(sizeof(Function));
    if (!f) return NULL;
    f->type = BUILT_IN;
    f->name = ton_strdup(name);
    f->body = NULL;         // No AST body for builtins
    f->closure_env = NULL;  // Not used for builtins currently
    return f;
}

// Math constants
const double TONLIB_PI = 3.14159265358979323846;
const double TONLIB_E = 2.71828182845904523536;

// Simple base64 implementation (public domain)
static const char base64_table[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char* base64_encode(const unsigned char* data, size_t input_length) {
    size_t output_length = 4 * ((input_length + 2) / 3);
    char* encoded_data = ton_malloc(output_length + 1);
    if (encoded_data == NULL) return NULL;

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        encoded_data[j++] = base64_table[(triple >> 18) & 0x3F];
        encoded_data[j++] = base64_table[(triple >> 12) & 0x3F];
        encoded_data[j++] = base64_table[(triple >> 6) & 0x3F];
        encoded_data[j++] = base64_table[triple & 0x3F];
    }

    // Add padding
    for (size_t i = 0; i < (3 - input_length % 3) % 3; i++) {
        encoded_data[output_length - 1 - i] = '=';
    }

    encoded_data[output_length] = '\0';
    return encoded_data;
}

unsigned char* base64_decode(const char* data, size_t* output_length) {
    size_t input_length = strlen(data);
    if (input_length % 4 != 0) return NULL;

    size_t padding = 0;
    if (input_length > 0 && data[input_length - 1] == '=') padding++;
    if (input_length > 1 && data[input_length - 2] == '=') padding++;

    *output_length = (input_length / 4) * 3 - padding;
    unsigned char* decoded_data = ton_malloc(*output_length);
    if (decoded_data == NULL) return NULL;

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : (size_t)(strchr(base64_table, data[i++]) - base64_table);
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : (size_t)(strchr(base64_table, data[i++]) - base64_table);
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : (size_t)(strchr(base64_table, data[i++]) - base64_table);
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : (size_t)(strchr(base64_table, data[i++]) - base64_table);

        uint32_t triple = (sextet_a << 18) | (sextet_b << 12) | (sextet_c << 6) | sextet_d;

        if (j < *output_length) decoded_data[j++] = (triple >> 16) & 0xFF;
        if (j < *output_length) decoded_data[j++] = (triple >> 8) & 0xFF;
        if (j < *output_length) decoded_data[j++] = triple & 0xFF;
    }

    return decoded_data;
}

// Helper function to convert binary data to hex string
static char* bin2hex(const unsigned char* data, size_t len) {
    if (data == NULL || len == 0) return NULL;
    
    char* hex = (char*)ton_malloc(len * 2 + 1);
    if (!hex) return NULL;
    
    for (size_t i = 0; i < len; i++) {
        sprintf(hex + i * 2, "%02x", data[i]);
    }
    hex[len * 2] = '\0';
    return hex;
}

#include "sha256.h"

// ==================== CRYPTOGRAPHIC FUNCTIONS IMPLEMENTATION ====================

// Hash functions (simplified versions)
Value tonlib_sha256(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) {
        return create_value_string("");
    }
    
    const char* input = args[0].data.string_val;
    unsigned char hash[SHA256_BLOCK_SIZE];
    SHA256_CTX ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, (const unsigned char*)input, strlen(input));
	sha256_final(&ctx, hash);
    
    char* hex_hash = bin2hex(hash, SHA256_BLOCK_SIZE);
    if (!hex_hash) {
        return create_value_string("");
    }
    Value result = create_value_string(hex_hash);
    ton_free(hex_hash);
    return result;
}

Value tonlib_md5(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) {
        return create_value_string("");
    }

    const char* input = args[0].data.string_val;
    unsigned char hash[16];

    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, input, strlen(input));
    MD5_Final(hash, &ctx);

    char* hex_hash = bin2hex(hash, 16);
    Value result = create_value_string(hex_hash);
    ton_free(hex_hash);
    return result;
}

// Base64 functions
Value tonlib_base64_encode(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) {
        return create_value_string("");
    }
    
    const char* input = args[0].data.string_val;
    char* encoded = base64_encode((const unsigned char*)input, strlen(input));
    Value result = create_value_string(encoded);
    ton_free(encoded);
    return result;
}

Value tonlib_base64_decode(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) {
        return create_value_string("");
    }
    
    const char* input = args[0].data.string_val;
    size_t decoded_len;
    unsigned char* decoded = base64_decode(input, &decoded_len);
    
    if (decoded == NULL) {
        return create_value_string("");
    }
    
    // Create string from decoded data
    char* result_str = ton_malloc(decoded_len + 1);
    memcpy(result_str, decoded, decoded_len);
    result_str[decoded_len] = '\0';
    
    Value result = create_value_string(result_str);
    ton_free(decoded);
    ton_free(result_str);
    return result;
}

// Random number generation
Value tonlib_random(Value* args, int arg_count) {
    (void)args;
    if (arg_count != 0) {
        return create_value_int(0);
    }
    
    // Simple random number between 0 and 1
    double random_value = (double)rand() / RAND_MAX;
    return create_value_float(random_value);
}

// Math functions
Value tonlib_math_pi(Value* args, int arg_count) {
    (void)args;
    (void)arg_count;
    return create_value_float(TONLIB_PI);
}

Value tonlib_math_e(Value* args, int arg_count) {
    (void)args;
    (void)arg_count;
    return create_value_float(TONLIB_E);
}

// Collection functions (using existing collections implementation)
Value tonlib_list_create(Value* args, int arg_count) {
    (void)args;
    (void)arg_count;
    TonList* list = tonlist_create();
    if (!list) {
        return create_value_error("Failed to create list");
    }
    return create_value_tonlist(list);
}

Value tonlib_map_create(Value* args, int arg_count) {
    (void)args;
    (void)arg_count;
    TonMap* map = tonmap_create();
    if (!map) {
        return create_value_error("Failed to create map");
    }
    return create_value_tonmap(map);
}

Value tonlib_set_create(Value* args, int arg_count) {
    (void)args;
    (void)arg_count;
    TonSet* set = tonset_create();
    if (!set) {
        return create_value_error("Failed to create set");
    }
    return create_value_tonset(set);
}

Value tonlib_array_create(Value* args, int arg_count) {
    // args[0] is the initial size, args[1] is the element type name (optional)
    size_t initial_size = 0;
    if (arg_count > 0 && args[0].type == VALUE_INT) {
        initial_size = (size_t)args[0].data.int_val;
    }

    TonArray* arr = create_dynamic_array(initial_size);
    if (arr == NULL) {
        return create_value_error("Failed to create array");
    }

    if (arg_count > 1 && args[1].type == VALUE_STRING) {
        arr->element_type_name = ton_strdup(args[1].data.string_val);
    }

    return create_value_array(arr);
}

// Type conversion functions
Value tonlib_int_to_string(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_INT) {
        return create_value_string("");
    }
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", args[0].data.int_val);
    return create_value_string(ton_strdup(buf));
}

Value tonlib_float_to_string(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_FLOAT) {
        return create_value_string("");
    }
    char buf[64];
    snprintf(buf, sizeof(buf), "%.6f", args[0].data.float_val);
    return create_value_string(ton_strdup(buf));
}

Value tonlib_string_to_int(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) {
        return create_value_int(0);
    }
    return create_value_int(atoi(args[0].data.string_val));
}

Value tonlib_string_to_float(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) {
        return create_value_float(0.0);
    }
    return create_value_float(atof(args[0].data.string_val));
}

// String operation functions
Value tonlib_length(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) {
        return create_value_int(0);
    }
    return create_value_int(strlen(args[0].data.string_val));
}

Value tonlib_concat(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_STRING || args[1].type != VALUE_STRING) {
        return create_value_string("");
    }
    size_t len1 = strlen(args[0].data.string_val);
    size_t len2 = strlen(args[1].data.string_val);
    char* new_str = ton_malloc(len1 + len2 + 1);
    strcpy(new_str, args[0].data.string_val);
    strcat(new_str, args[1].data.string_val);
    return create_value_string(new_str);
}

Value tonlib_substring(Value* args, int arg_count) {
    if (arg_count != 3 || args[0].type != VALUE_STRING || args[1].type != VALUE_INT || args[2].type != VALUE_INT) {
        return create_value_string("");
    }
    const char* str = args[0].data.string_val;
    int start = args[1].data.int_val;
    int len = args[2].data.int_val;
    if (start < 0 || len < 0 || start >= (int)strlen(str)) {
        return create_value_string("");
    }
    char* sub = ton_malloc(len + 1);
    strncpy(sub, str + start, len);
    sub[len] = '\0';
    return create_value_string(sub);
}

// Add more string functions as needed...

Value tonlib_string_to_int_base(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_STRING || args[1].type != VALUE_INT) {
        return create_value_int(0);
    }
    long val = strtol(args[0].data.string_val, NULL, args[1].data.int_val);
    return create_value_int((int)val);
}

Value tonlib_int_to_hex_upper(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_INT) {
        return create_value_string("");
    }
    char buf[32];
    snprintf(buf, sizeof(buf), "%X", args[0].data.int_val);
    return create_value_string(ton_strdup(buf));
}

Value tonlib_upper_case(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) {
        return create_value_string("");
    }
    size_t len = strlen(args[0].data.string_val);
    char* str = ton_malloc(len + 1);
    strcpy(str, args[0].data.string_val);
    for (size_t i = 0; i < len; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
    return create_value_string(str);
}

Value tonlib_lower_case(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) {
        return create_value_string("");
    }
    size_t len = strlen(args[0].data.string_val);
    char* str = ton_malloc(len + 1);
    strcpy(str, args[0].data.string_val);
    for (size_t i = 0; i < len; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
    return create_value_string(str);
}

Value tonlib_strpos(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_STRING || args[1].type != VALUE_STRING) {
        return create_value_int(-1);
    }
    const char* haystack = args[0].data.string_val;
    const char* needle = args[1].data.string_val;
    char* pos = strstr(haystack, needle);
    if (pos) {
        return create_value_int((int)(pos - haystack));
    }
    return create_value_int(-1);
}

// Install builtin functions
void install_tonlib_builtins(Environment* env) {
    // Hash functions
    env_add_function(env, "sha256", make_builtin_fn("sha256"));
    env_add_function(env, "md5", make_builtin_fn("md5"));
    
    // Encoding functions
    env_add_function(env, "base64_encode", make_builtin_fn("base64_encode"));
    env_add_function(env, "base64_decode", make_builtin_fn("base64_decode"));
    
    // Math functions
    env_add_function(env, "math_pi", make_builtin_fn("math_pi"));
    env_add_function(env, "math_e", make_builtin_fn("math_e"));
    env_add_function(env, "random", make_builtin_fn("random"));
    
    // Collection functions
    env_add_function(env, "list_create", make_builtin_fn("list_create"));
    env_add_function(env, "map_create", make_builtin_fn("map_create"));
    env_add_function(env, "set_create", make_builtin_fn("set_create"));
    env_add_function(env, "array_create", make_builtin_fn("array_create")); // Dodano array_create
    
    // Type conversions
    env_add_function(env, "int_to_string", make_builtin_fn("int_to_string"));
    env_add_function(env, "float_to_string", make_builtin_fn("float_to_string"));
    env_add_function(env, "string_to_int", make_builtin_fn("string_to_int"));
    env_add_function(env, "string_to_float", make_builtin_fn("string_to_float"));
    
    // String operations
    env_add_function(env, "length", make_builtin_fn("length"));
    env_add_function(env, "concat", make_builtin_fn("concat"));
    env_add_function(env, "substring", make_builtin_fn("substring"));
    env_add_function(env, "string_to_int_base", make_builtin_fn("string_to_int_base"));
    env_add_function(env, "int_to_hex_upper", make_builtin_fn("int_to_hex_upper"));
    env_add_function(env, "upper_case", make_builtin_fn("upper_case"));
    env_add_function(env, "lower_case", make_builtin_fn("lower_case"));
    env_add_function(env, "strpos", make_builtin_fn("strpos"));
}

// Update call_tonlib_function
Value call_tonlib_function(const char* function_name, Value* args, int arg_count) {
    if (strcmp(function_name, "sha256") == 0) {
        return tonlib_sha256(args, arg_count);
    } else if (strcmp(function_name, "md5") == 0) {
        return tonlib_md5(args, arg_count);
    } else if (strcmp(function_name, "base64_encode") == 0) {
        return tonlib_base64_encode(args, arg_count);
    } else if (strcmp(function_name, "base64_decode") == 0) {
        return tonlib_base64_decode(args, arg_count);
    } else if (strcmp(function_name, "random") == 0) {
        return tonlib_random(args, arg_count);
    } else if (strcmp(function_name, "math_pi") == 0) {
        return tonlib_math_pi(args, arg_count);
    } else if (strcmp(function_name, "math_e") == 0) {
        return tonlib_math_e(args, arg_count);
    } else if (strcmp(function_name, "list_create") == 0) {
        return tonlib_list_create(args, arg_count);
    } else if (strcmp(function_name, "map_create") == 0) {
        return tonlib_map_create(args, arg_count);
    } else if (strcmp(function_name, "set_create") == 0) {
        return tonlib_set_create(args, arg_count);
    } else if (strcmp(function_name, "array_create") == 0) {
        return tonlib_array_create(args, arg_count);
    } else if (strcmp(function_name, "int_to_string") == 0) {
        return tonlib_int_to_string(args, arg_count);
    } else if (strcmp(function_name, "float_to_string") == 0) {
        return tonlib_float_to_string(args, arg_count);
    } else if (strcmp(function_name, "string_to_int") == 0) {
        return tonlib_string_to_int(args, arg_count);
    } else if (strcmp(function_name, "string_to_float") == 0) {
        return tonlib_string_to_float(args, arg_count);
    } else if (strcmp(function_name, "length") == 0) {
        return tonlib_length(args, arg_count);
    } else if (strcmp(function_name, "concat") == 0) {
        return tonlib_concat(args, arg_count);
    } else if (strcmp(function_name, "substring") == 0) {
        return tonlib_substring(args, arg_count);
    } else if (strcmp(function_name, "string_to_int_base") == 0) {
        return tonlib_string_to_int_base(args, arg_count);
    } else if (strcmp(function_name, "int_to_hex_upper") == 0) {
        return tonlib_int_to_hex_upper(args, arg_count);
    } else if (strcmp(function_name, "upper_case") == 0) {
        return tonlib_upper_case(args, arg_count);
    } else if (strcmp(function_name, "lower_case") == 0) {
        return tonlib_lower_case(args, arg_count);
    } else if (strcmp(function_name, "strpos") == 0) {
        return tonlib_strpos(args, arg_count);
    }
    // Placeholder
    return create_value_int(0);
}