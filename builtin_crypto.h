#ifndef TON_BUILTIN_CRYPTO_H
#define TON_BUILTIN_CRYPTO_H

#include "interpreter.h"
#include "environment.h"

// Crypto module initialization
void install_crypto_builtins(Environment* env);

// Crypto function dispatcher
Value call_crypto_function(const char* function_name, Value* args, int arg_count);

// Hash functions
Value crypto_sha256_hash(Value* args, int arg_count);
Value crypto_md5_hash(Value* args, int arg_count);

// Encoding functions
Value crypto_base64_encode_text(Value* args, int arg_count);
Value crypto_base64_decode_text(Value* args, int arg_count);

// Random functions
Value crypto_random_int(Value* args, int arg_count);
Value crypto_random_string(Value* args, int arg_count);

// String manipulation functions
Value crypto_xor_strings(Value* args, int arg_count);
Value crypto_caesar_encrypt(Value* args, int arg_count);
Value crypto_caesar_decrypt(Value* args, int arg_count);
Value crypto_rot13(Value* args, int arg_count);

// Character functions
Value crypto_char_code(Value* args, int arg_count);
Value crypto_char_from_code(Value* args, int arg_count);

// Demo function
Value crypto_demo(Value* args, int arg_count);

#endif // TON_BUILTIN_CRYPTO_H