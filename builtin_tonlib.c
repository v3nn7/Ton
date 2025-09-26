#include "builtin_tonlib.h"
#include "builtin.h"
#include "collections.h"
#include "sha256.h"
#include "md5.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>

// Helper function to create builtin function stubs
static Function* make_builtin_fn(const char* name) {
    Function* f = (Function*)malloc(sizeof(Function));
    if (!f) return NULL;
    f->name = _strdup(name);
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
    char* encoded_data = malloc(output_length + 1);
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
    unsigned char* decoded_data = malloc(*output_length);
    if (decoded_data == NULL) return NULL;

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : strchr(base64_table, data[i++]) - base64_table;
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : strchr(base64_table, data[i++]) - base64_table;
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : strchr(base64_table, data[i++]) - base64_table;
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : strchr(base64_table, data[i++]) - base64_table;

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
    
    char* hex = (char*)malloc(len * 2 + 1);
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
    Value result = create_value_string(hex_hash);
    free(hex_hash);
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
    free(hex_hash);
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
    free(encoded);
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
    char* result_str = malloc(decoded_len + 1);
    memcpy(result_str, decoded, decoded_len);
    result_str[decoded_len] = '\0';
    
    Value result = create_value_string(result_str);
    free(decoded);
    free(result_str);
    return result;
}

// Random number generation
Value tonlib_random(Value* args, int arg_count) {
    if (arg_count != 0) {
        return create_value_int(0);
    }
    
    // Simple random number between 0 and 1
    double random_value = (double)rand() / RAND_MAX;
    return create_value_float(random_value);
}

// Math functions
Value tonlib_math_pi(Value* args, int arg_count) {
    return create_value_float(TONLIB_PI);
}

Value tonlib_math_e(Value* args, int arg_count) {
    return create_value_float(TONLIB_E);
}

// Collection functions (using existing collections implementation)
Value tonlib_list_create(Value* args, int arg_count) {
    // Placeholder - would create a list collection
    return create_value_int(0);
}

Value tonlib_map_create(Value* args, int arg_count) {
    // Placeholder - would create a map collection
    return create_value_int(0);
}

Value tonlib_set_create(Value* args, int arg_count) {
    // Placeholder - would create a set collection
    return create_value_int(0);
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
}

// Call tonlib function (simplified)
Value call_tonlib_function(const char* function_name, ASTNode** arguments, int arg_count, Environment* env) {
    // Placeholder implementation
    // In real implementation, this would parse arguments and call the appropriate function
    return create_value_int(0);
}