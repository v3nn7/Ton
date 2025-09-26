#ifndef TON_BUILTIN_TONLIB_H
#define TON_BUILTIN_TONLIB_H

#include "interpreter.h"
#include "environment.h"

// TonLib version information
#define TONLIB_VERSION "1.0.0"
#define TONLIB_AUTHOR "v3nn7"

// TonLib initialization and info functions
void install_tonlib_builtins(Environment* env);
Value call_tonlib_function(const char* function_name, ASTNode** arguments, int arg_count, Environment* env);

// Core module functions
Value tonlib_type_of(Value* args, int arg_count);
Value tonlib_is_null(Value* args, int arg_count);
Value tonlib_int_to_string(Value* args, int arg_count);
Value tonlib_float_to_string(Value* args, int arg_count);
Value tonlib_string_to_int(Value* args, int arg_count);
Value tonlib_string_to_float(Value* args, int arg_count);
Value tonlib_bool_to_string(Value* args, int arg_count);
Value tonlib_string_to_bool(Value* args, int arg_count);

// Math module functions
Value tonlib_abs(Value* args, int arg_count);
Value tonlib_pow(Value* args, int arg_count);
Value tonlib_sqrt(Value* args, int arg_count);
Value tonlib_sin(Value* args, int arg_count);
Value tonlib_cos(Value* args, int arg_count);
Value tonlib_tan(Value* args, int arg_count);
Value tonlib_floor(Value* args, int arg_count);
Value tonlib_ceil(Value* args, int arg_count);
Value tonlib_round(Value* args, int arg_count);
Value tonlib_min(Value* args, int arg_count);
Value tonlib_max(Value* args, int arg_count);
Value tonlib_random(Value* args, int arg_count);
Value tonlib_random_int(Value* args, int arg_count);

// String module functions
Value tonlib_length(Value* args, int arg_count);
Value tonlib_concat(Value* args, int arg_count);
Value tonlib_substring(Value* args, int arg_count);
Value tonlib_index_of(Value* args, int arg_count);
Value tonlib_to_upper(Value* args, int arg_count);
Value tonlib_to_lower(Value* args, int arg_count);
Value tonlib_trim(Value* args, int arg_count);
Value tonlib_contains(Value* args, int arg_count);
Value tonlib_starts_with(Value* args, int arg_count);
Value tonlib_ends_with(Value* args, int arg_count);
Value tonlib_replace(Value* args, int arg_count);
Value tonlib_split(Value* args, int arg_count);
Value tonlib_join(Value* args, int arg_count);
Value tonlib_is_empty(Value* args, int arg_count);

// Memory management functions
Value tonlib_alloc(Value* args, int arg_count);
Value tonlib_free(Value* args, int arg_count);

// File I/O functions
Value tonlib_file_read_all(Value* args, int arg_count);
Value tonlib_file_write_all(Value* args, int arg_count);

// Collections functions - TonList
Value tonlib_list_create(Value* args, int arg_count);
Value tonlib_list_push(Value* args, int arg_count);
Value tonlib_list_pop(Value* args, int arg_count);
Value tonlib_list_get(Value* args, int arg_count);
Value tonlib_list_set(Value* args, int arg_count);
Value tonlib_list_size(Value* args, int arg_count);

Value tonlib_map_create(Value* args, int arg_count);
Value tonlib_map_set(Value* args, int arg_count);
Value tonlib_map_get(Value* args, int arg_count);
Value tonlib_map_has(Value* args, int arg_count);
Value tonlib_map_remove(Value* args, int arg_count);
Value tonlib_map_size(Value* args, int arg_count);

Value tonlib_set_create(Value* args, int arg_count);
Value tonlib_set_add(Value* args, int arg_count);
Value tonlib_set_has(Value* args, int arg_count);
Value tonlib_set_remove(Value* args, int arg_count);
Value tonlib_set_size(Value* args, int arg_count);

// TonLib info functions
Value* tonlib_init(Value* args, int arg_count);
Value tonlib_version(Value* args, int arg_count);
Value tonlib_author(Value* args, int arg_count);
Value* tonlib_info(Value* args, int arg_count);

// Math constants
extern const double TONLIB_PI;
extern const double TONLIB_E;

#endif // TON_BUILTIN_TONLIB_H