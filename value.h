#ifndef VALUE_H
#define VALUE_H

// Forward declaration for Function to avoid circular dependency
typedef struct Function Function;

// Value type for interpreter
typedef enum {
    VALUE_INT,
    VALUE_FN,
    VALUE_STRING,
    VALUE_FLOAT,
    VALUE_BOOL,
    VALUE_NULL,
    VALUE_POINTER,
    VALUE_ARRAY,
    VALUE_TONLIST,
    VALUE_TONMAP,
    VALUE_TONSET
} ValueType;

typedef struct Value {
    ValueType type;
    union {
        int int_val;           // Renamed for consistency with TonLib
        Function* function_value; // Changed to pointer to avoid incomplete type
        char* string_val;      // Renamed for consistency with TonLib
        double float_val;      // Add float value
        int bool_val;          // Add bool value (using int for compatibility)
        void* pointer_val;     // Add pointer value for low-level programming
        void* array_val;       // TonArray pointer
        void* tonlist_val;     // TonList pointer
        void* tonmap_val;      // TonMap pointer
        void* tonset_val;      // TonSet pointer
    } data;                    // Wrapped in data union for consistency with TonLib
} Value;

// Function prototypes for value creation
Value create_value_int(int i);
Value create_value_fn(Function* fn); // Changed to pointer
Value create_value_string(char* s);
Value create_value_float(double f);
Value create_value_bool(int b);
Value create_value_null(void);
Value create_value_pointer(void* ptr);
Value create_value_array(void* arr);
Value create_value_tonlist(void* list);
Value create_value_tonmap(void* map);
Value create_value_tonset(void* set);

#endif // VALUE_H