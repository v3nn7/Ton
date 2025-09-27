#include "tonlib_low.h"
#include "builtin.h"
#include "value.h"
#include "error.h"
#include "memory.h"
#include "collections.h" // Dodano dla list_value_empty i list_value_add
#include "environment.h" // Dodano dla environment_define_builtin

// Wrapper for rdtsc_wrapper
Value ton_rdtsc(int arg_count, Value *args)
{
    if (arg_count != 0) {
        ton_error(TON_ERR_RUNTIME, "rdtsc() takes no arguments.", 0, 0, __FILE__);
        return create_value_null();
    }
    uint64_t tsc = rdtsc_wrapper();
    return create_value_float((double)tsc); // Zmieniono na create_value_float
}

// Wrapper for cpuid_wrapper
Value ton_cpuid(int arg_count, Value *args)
{
    if (arg_count != 1) {
        ton_error(TON_ERR_RUNTIME, "cpuid() takes 1 argument (eax_in).", 0, 0, __FILE__);
        return create_value_null();
    }
    if (args[0].type != VALUE_INT) { // Zmieniono na VALUE_INT
        ton_error(TON_ERR_RUNTIME, "cpuid() argument must be an integer.", 0, 0, __FILE__); // Zmieniono komunikat
        return create_value_null();
    }

    uint32_t eax_in = (uint32_t)args[0].data.int_val; // Zmieniono na data.int_val
    uint32_t eax_out, ebx_out, ecx_out, edx_out;

    cpuid_wrapper(eax_in, &eax_out, &ebx_out, &ecx_out, &edx_out);

    // Return a list of 4 numbers
    Value result_list = create_value_tonlist(tonlist_create()); // Używamy tonlist_create
    tonlist_push((TonList*)result_list.data.tonlist_val, create_value_int((int)eax_out)); // Używamy tonlist_push
    tonlist_push((TonList*)result_list.data.tonlist_val, create_value_int((int)ebx_out)); // Używamy tonlist_push
    tonlist_push((TonList*)result_list.data.tonlist_val, create_value_int((int)ecx_out)); // Używamy tonlist_push
    tonlist_push((TonList*)result_list.data.tonlist_val, create_value_int((int)edx_out)); // Używamy tonlist_push

    return result_list;
}

// Wrapper for popcnt_wrapper
Value ton_popcnt(int arg_count, Value *args)
{
    if (arg_count != 1) {
        ton_error(TON_ERR_RUNTIME, "popcnt() takes 1 argument (value).", 0, 0, __FILE__);
        return create_value_null();
    }
    if (args[0].type != VALUE_INT) {
        ton_error(TON_ERR_RUNTIME, "popcnt() argument must be an integer.", 0, 0, __FILE__);
        return create_value_null();
    }
    uint64_t value = (uint64_t)args[0].data.int_val;
    uint64_t result = popcnt_wrapper(value);
    return create_value_int((int)result);
}

// Wrapper for bsf_wrapper
Value ton_bsf(int arg_count, Value *args)
{
    if (arg_count != 1) {
        ton_error(TON_ERR_RUNTIME, "bsf() takes 1 argument (value).", 0, 0, __FILE__);
        return create_value_null();
    }
    if (args[0].type != VALUE_INT) {
        ton_error(TON_ERR_RUNTIME, "bsf() argument must be an integer.", 0, 0, __FILE__);
        return create_value_null();
    }
    uint64_t value = (uint64_t)args[0].data.int_val;
    int64_t result = bsf_wrapper(value);
    return create_value_int((int)result);
}

// Wrapper for bsr_wrapper
Value ton_bsr(int arg_count, Value *args)
{
    if (arg_count != 1) {
        ton_error(TON_ERR_RUNTIME, "bsr() takes 1 argument (value).", 0, 0, __FILE__);
        return create_value_null();
    }
    if (args[0].type != VALUE_INT) {
        ton_error(TON_ERR_RUNTIME, "bsr() argument must be an integer.", 0, 0, __FILE__);
        return create_value_null();
    }
    uint64_t value = (uint64_t)args[0].data.int_val;
    int64_t result = bsr_wrapper(value);
    return create_value_int((int)result);
}

// Function to register low-level built-in functions
void register_tonlib_low_functions(Environment *env)
{
    env_add_function(env, "rdtsc", make_builtin_fn("rdtsc"));
    env_add_function(env, "cpuid", make_builtin_fn("cpuid"));
    env_add_function(env, "popcnt", make_builtin_fn("popcnt"));
    env_add_function(env, "bsf", make_builtin_fn("bsf"));
    env_add_function(env, "bsr", make_builtin_fn("bsr"));
}