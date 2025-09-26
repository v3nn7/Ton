#ifndef VALUE_H
#define VALUE_H

// Forward declaration for Function to avoid circular dependency
typedef struct Function Function;

// Forward declaration for Value
typedef struct Value Value;

// Structure for method data
typedef struct {
    Value* object;  // Use pointer to avoid incomplete type
    char* method_name;
} MethodData;

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
    VALUE_TONSET,
    VALUE_METHOD,
    VALUE_ERROR
} ValueType;

typedef struct Value {
    ValueType type;
    int ref_count;  // Add reference counter
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
        MethodData method_val; // Method data for object method calls
        char* error_message;
    } data;                    // Wrapped in data union for consistency with TonLib
} Value;

// Function prototypes for value creation
Value create_value_int(int i);
Value create_value_fn(Function* fn); // Changed to pointer
Value create_value_string(const char* s);
Value create_value_float(double f);
Value create_value_bool(int b);
Value create_value_null(void);
Value create_value_pointer(void* ptr);
Value create_value_array(void* arr);
Value create_value_tonlist(void* list);
Value create_value_tonmap(void* map);
Value create_value_tonset(void* set);
Value create_value_method(Value* object, char* method_name);
Value create_value_error(const char* message);

// Memory management functions
void value_add_ref(Value* val);
void value_release(Value* val);
char* value_to_string(Value* val);
const char* value_type_to_string(ValueType type);

#endif // VALUE_H