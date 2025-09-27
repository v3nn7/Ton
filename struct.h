#ifndef TON_STRUCT_H
#define TON_STRUCT_H


#include <stddef.h>

// Forward declarations
typedef struct FunctionDeclarationNode FunctionDeclarationNode;
typedef struct Value Value;

// Access modifiers for OOP
typedef enum {
    ACCESS_PUBLIC,
    ACCESS_PROTECTED,
    ACCESS_PRIVATE
} AccessModifier;

typedef struct StructField {
    const char* name;
    const char* type_name;
    AccessModifier access;  // New: access modifier
} StructField;

typedef struct StructMethod {
    const char* name;
    FunctionDeclarationNode* function;
    AccessModifier access;  // New: access modifier
    int is_virtual;         // New: for polymorphism
    int is_constructor;     // New: constructor flag
    int is_destructor;      // New: destructor flag
} StructMethod;

typedef struct TonStructType {
    const char* name;
    const char* parent_name;    // New: for inheritance
    struct TonStructType* parent; // New: parent class pointer
    int         num_fields;
    StructField* fields;
    // Methods support
    int num_methods;
    StructMethod* methods;
    // OOP support
    StructMethod* constructor;   // New: default constructor
    StructMethod* destructor;    // New: destructor
    StructMethod** vtable;       // New: virtual method table
    int vtable_size;             // New: vtable size
    size_t total_size;           // New: total size of instance including type pointer
} TonStructType;

typedef struct TonStructInstance {
    const TonStructType* type;
    Value* field_values;
} TonStructInstance;

TonStructType* define_struct_type(const char* name, StructField* fields, int num_fields, StructMethod* methods, int num_methods);
void           destroy_struct_type(TonStructType* t);

TonStructInstance* create_struct_instance(const TonStructType* t);
void               destroy_struct_instance(TonStructInstance* si);

int   struct_set_field(TonStructInstance* si, const char* field_name, Value v);
Value struct_get_field(const TonStructInstance* si, const char* field_name);

// Method support functions
FunctionDeclarationNode* struct_get_method(const TonStructType* t, const char* method_name);
Value struct_call_method(TonStructInstance* si, const char* method_name, Value* args, int num_args);

// OOP helper functions
int check_field_access(const TonStructType* type, const char* field_name, AccessModifier required_access);
int check_method_access(const TonStructType* type, const char* method_name, AccessModifier required_access);
StructMethod* find_method_in_hierarchy(const TonStructType* type, const char* method_name);
TonStructInstance* create_struct_with_constructor(const TonStructType* t, Value* args, int num_args);
void destroy_struct_with_destructor(TonStructInstance* si);

// Function to find struct type by name
TonStructType* find_struct_type(const char* name);

#endif // TON_STRUCT_H