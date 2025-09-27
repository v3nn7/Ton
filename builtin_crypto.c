/*
 * Ton Crypto Library - Built-in Implementation
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "builtin_crypto.h"
#include "builtin.h"
#include "sha256.h"
#include "md5.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <ctype.h>

// Helper function to create builtin function stubs
static Function* make_crypto_builtin_fn(const char* name) {
    Function* f = (Function*)ton_malloc(sizeof(Function));
    if (!f) return NULL;
    f->type = BUILT_IN;
    f->name = ton_strdup(name);
    f->body = NULL;
    f->closure_env = NULL;
    return f;
}

// Base64 encoding table
static const char base64_table[] = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// Base64 encode function
static char* base64_encode_internal(const unsigned char* data, size_t input_length) {
    size_t output_length = 4 * ((input_length + 2) / 3);
    char* encoded_data = ton_malloc(output_length + 1);
    if (encoded_data == NULL) return NULL;

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        encoded_data[j++] = base64_table[(triple >> 18) & 63];
        encoded_data[j++] = base64_table[(triple >> 12) & 63];
        encoded_data[j++] = base64_table[(triple >> 6) & 63];
        encoded_data[j++] = base64_table[triple & 63];
    }

    for (int i = 0; i < (3 - input_length % 3) % 3; i++)
        encoded_data[output_length - 1 - i] = '=';

    encoded_data[output_length] = '\0';
    return encoded_data;
}

// Base64 decode function
static unsigned char* base64_decode_internal(const char* data, size_t input_length, size_t* output_length) {
    if (input_length % 4 != 0) return NULL;

    *output_length = input_length / 4 * 3;
    if (data[input_length - 1] == '=') (*output_length)--;
    if (data[input_length - 2] == '=') (*output_length)--;

    unsigned char* decoded_data = ton_malloc(*output_length);
    if (decoded_data == NULL) return NULL;

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : strchr(base64_table, data[i++]) - base64_table;
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : strchr(base64_table, data[i++]) - base64_table;
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : strchr(base64_table, data[i++]) - base64_table;
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : strchr(base64_table, data[i++]) - base64_table;

        uint32_t triple = (sextet_a << 18) | (sextet_b << 12) | (sextet_c << 6) | sextet_d;

        if (j < *output_length) decoded_data[j++] = (triple >> 16) & 255;
        if (j < *output_length) decoded_data[j++] = (triple >> 8) & 255;
        if (j < *output_length) decoded_data[j++] = triple & 255;
    }

    return decoded_data;
}

// Hash functions
Value crypto_sha256_hash(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) {
        return create_value_error("sha256_hash requires exactly one string argument");
    }
    
    const char* input = args[0].data.string_val;
    
    SHA256_CTX ctx;
    BYTE hash[SHA256_BLOCK_SIZE];
    
    sha256_init(&ctx);
    sha256_update(&ctx, (const BYTE*)input, strlen(input));
    sha256_final(&ctx, hash);
    
    // Convert to hex string
    char* hex_string = (char*)ton_malloc(SHA256_BLOCK_SIZE * 2 + 1);
    if (!hex_string) {
        return create_value_error("Memory allocation failed");
    }
    
    for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
        sprintf(hex_string + i * 2, "%02x", hash[i]);
    }
    hex_string[SHA256_BLOCK_SIZE * 2] = '\0';
    
    Value result = create_value_string(hex_string);
    ton_free(hex_string);
    return result;
}

Value crypto_md5_hash(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) {
        return create_value_error("md5_hash requires exactly one string argument");
    }
    
    const char* input = args[0].data.string_val;
    
    MD5_CTX ctx;
    unsigned char hash[16];
    
    MD5_Init(&ctx);
    MD5_Update(&ctx, input, strlen(input));
    MD5_Final(hash, &ctx);
    
    // Convert to hex string
    char* hex_string = (char*)ton_malloc(32 + 1);
    if (!hex_string) {
        return create_value_error("Memory allocation failed");
    }
    
    for (int i = 0; i < 16; i++) {
        sprintf(hex_string + i * 2, "%02x", hash[i]);
    }
    hex_string[32] = '\0';
    
    Value result = create_value_string(hex_string);
    ton_free(hex_string);
    return result;
}

// Encoding functions
Value crypto_base64_encode_text(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) {
        return create_value_string("");
    }
    
    const char* input = args[0].data.string_val;
    char* encoded = base64_encode_internal((const unsigned char*)input, strlen(input));
    Value result = create_value_string(encoded);
    ton_free(encoded);
    return result;
}

Value crypto_base64_decode_text(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) {
        return create_value_string("");
    }
    
    const char* input = args[0].data.string_val;
    size_t output_length;
    unsigned char* decoded = base64_decode_internal(input, strlen(input), &output_length);
    
    if (!decoded) {
        return create_value_string("");
    }
    
    char* result_str = ton_malloc(output_length + 1);
    memcpy(result_str, decoded, output_length);
    result_str[output_length] = '\0';
    
    Value result = create_value_string(result_str);
    ton_free(decoded);
    ton_free(result_str);
    return result;
}

// Random functions
Value crypto_random_int(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_INT || args[1].type != VALUE_INT) {
        return create_value_int(0);
    }
    
    int min = args[0].data.int_val;
    int max = args[1].data.int_val;
    
    if (min > max) {
        int temp = min;
        min = max;
        max = temp;
    }
    
    return create_value_int(min + (rand() % (max - min + 1)));
}

Value crypto_random_string(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_INT) {
        return create_value_string("");
    }
    
    int length = args[0].data.int_val;
    if (length <= 0) {
        return create_value_string("");
    }
    
    const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    char* result = ton_malloc(length + 1);
    
    for (int i = 0; i < length; i++) {
        result[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    result[length] = '\0';
    
    Value val = create_value_string(result);
    ton_free(result);
    return val;
}

// String manipulation functions
Value crypto_xor_strings(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_STRING || args[1].type != VALUE_STRING) {
        return create_value_string("");
    }
    
    const char* str1 = args[0].data.string_val;
    const char* str2 = args[1].data.string_val;
    
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    
    if (len1 == 0 || len2 == 0) {
        return create_value_string("");
    }
    
    int max_len = len1 > len2 ? len1 : len2;
    char* result = ton_malloc(max_len + 1);
    
    for (int i = 0; i < max_len; i++) {
        char c1 = (i < len1) ? str1[i] : 0;
        char c2 = (i < len2) ? str2[i] : 0;
        result[i] = c1 ^ c2;
    }
    result[max_len] = '\0';
    
    Value val = create_value_string(result);
    ton_free(result);
    return val;
}

Value crypto_caesar_encrypt(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_STRING || args[1].type != VALUE_INT) {
        return create_value_string("");
    }
    
    const char* text = args[0].data.string_val;
    int shift = args[1].data.int_val % 26;
    if (shift < 0) shift += 26;
    
    int len = strlen(text);
    char* result = ton_malloc(len + 1);
    
    for (int i = 0; i < len; i++) {
        char c = text[i];
        if (c >= 'A' && c <= 'Z') {
            result[i] = 'A' + (c - 'A' + shift) % 26;
        } else if (c >= 'a' && c <= 'z') {
            result[i] = 'a' + (c - 'a' + shift) % 26;
        } else {
            result[i] = c;
        }
    }
    result[len] = '\0';
    
    Value val = create_value_string(result);
    ton_free(result);
    return val;
}

Value crypto_caesar_decrypt(Value* args, int arg_count) {
    if (arg_count != 2 || args[0].type != VALUE_STRING || args[1].type != VALUE_INT) {
        return create_value_string("");
    }
    
    const char* text = args[0].data.string_val;
    int shift = -(args[1].data.int_val % 26);
    if (shift < 0) shift += 26;
    
    int len = strlen(text);
    char* result = ton_malloc(len + 1);
    
    for (int i = 0; i < len; i++) {
        char c = text[i];
        if (c >= 'A' && c <= 'Z') {
            result[i] = 'A' + (c - 'A' + shift) % 26;
        } else if (c >= 'a' && c <= 'z') {
            result[i] = 'a' + (c - 'a' + shift) % 26;
        } else {
            result[i] = c;
        }
    }
    result[len] = '\0';
    
    Value val = create_value_string(result);
    ton_free(result);
    return val;
}

Value crypto_rot13(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) {
        return create_value_string("");
    }
    
    const char* text = args[0].data.string_val;
    int len = strlen(text);
    char* result = ton_malloc(len + 1);
    
    for (int i = 0; i < len; i++) {
        char c = text[i];
        if (c >= 'A' && c <= 'Z') {
            result[i] = 'A' + (c - 'A' + 13) % 26;
        } else if (c >= 'a' && c <= 'z') {
            result[i] = 'a' + (c - 'a' + 13) % 26;
        } else {
            result[i] = c;
        }
    }
    result[len] = '\0';
    
    Value val = create_value_string(result);
    ton_free(result);
    return val;
}

// Character functions
Value crypto_char_code(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_STRING) {
        return create_value_int(0);
    }
    
    const char* str = args[0].data.string_val;
    if (strlen(str) == 0) {
        return create_value_int(0);
    }
    
    return create_value_int((int)(unsigned char)str[0]);
}

Value crypto_char_from_code(Value* args, int arg_count) {
    if (arg_count != 1 || args[0].type != VALUE_INT) {
        return create_value_string("");
    }
    
    int code = args[0].data.int_val;
    if (code < 0 || code > 255) {
        return create_value_string("");
    }
    
    char result[2] = {(char)code, '\0'};
    return create_value_string(result);
}

// Demo function
Value crypto_demo(Value* args, int arg_count) {
    (void)args;
    (void)arg_count;
    return create_value_string("Crypto module loaded successfully!");
}

// Crypto function dispatcher
Value call_crypto_function(const char* function_name, Value* args, int arg_count) {
    if (strcmp(function_name, "sha256_hash") == 0) {
        return crypto_sha256_hash(args, arg_count);
    } else if (strcmp(function_name, "md5_hash") == 0) {
        return crypto_md5_hash(args, arg_count);
    } else if (strcmp(function_name, "base64_encode_text") == 0) {
        return crypto_base64_encode_text(args, arg_count);
    } else if (strcmp(function_name, "base64_decode_text") == 0) {
        return crypto_base64_decode_text(args, arg_count);
    } else if (strcmp(function_name, "random_int") == 0) {
        return crypto_random_int(args, arg_count);
    } else if (strcmp(function_name, "random_string") == 0) {
        return crypto_random_string(args, arg_count);
    } else if (strcmp(function_name, "xor_strings") == 0) {
        return crypto_xor_strings(args, arg_count);
    } else if (strcmp(function_name, "caesar_encrypt") == 0) {
        return crypto_caesar_encrypt(args, arg_count);
    } else if (strcmp(function_name, "caesar_decrypt") == 0) {
        return crypto_caesar_decrypt(args, arg_count);
    } else if (strcmp(function_name, "rot13") == 0) {
        return crypto_rot13(args, arg_count);
    } else if (strcmp(function_name, "char_code") == 0) {
        return crypto_char_code(args, arg_count);
    } else if (strcmp(function_name, "char_from_code") == 0) {
        return crypto_char_from_code(args, arg_count);
    } else if (strcmp(function_name, "demo") == 0) {
        return crypto_demo(args, arg_count);
    }
    
    // Return error for unknown function
    return create_value_error("Unknown crypto function");
}

// Install crypto builtin functions
void install_crypto_builtins(Environment* env) {
    // Hash functions
    env_add_function(env, "sha256_hash", make_crypto_builtin_fn("sha256_hash"));
    env_add_function(env, "md5_hash", make_crypto_builtin_fn("md5_hash"));
    
    // Encoding functions
    env_add_function(env, "base64_encode_text", make_crypto_builtin_fn("base64_encode_text"));
    env_add_function(env, "base64_decode_text", make_crypto_builtin_fn("base64_decode_text"));
    
    // Random functions
    env_add_function(env, "random_int", make_crypto_builtin_fn("random_int"));
    env_add_function(env, "random_string", make_crypto_builtin_fn("random_string"));
    
    // String manipulation functions
    env_add_function(env, "xor_strings", make_crypto_builtin_fn("xor_strings"));
    env_add_function(env, "caesar_encrypt", make_crypto_builtin_fn("caesar_encrypt"));
    env_add_function(env, "caesar_decrypt", make_crypto_builtin_fn("caesar_decrypt"));
    env_add_function(env, "rot13", make_crypto_builtin_fn("rot13"));
    
    // Character functions
    env_add_function(env, "char_code", make_crypto_builtin_fn("char_code"));
    env_add_function(env, "char_from_code", make_crypto_builtin_fn("char_from_code"));
    
    // Demo function
    env_add_function(env, "crypto_demo", make_crypto_builtin_fn("crypto_demo"));
}