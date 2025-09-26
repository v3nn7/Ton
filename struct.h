#ifndef TON_STRUCT_H
#define TON_STRUCT_H

#include "interpreter.h"

// Forward declaration
typedef struct FunctionDeclarationNode FunctionDeclarationNode;

typedef struct StructField {
    const char* name;
    const char* type_name;
} StructField;

typedef struct StructMethod {
    const char* name;
    FunctionDeclarationNode* function;
} StructMethod;

typedef struct TonStructType {
    const char* name;
    int         num_fields;
    StructField* fields;
    // Methods support
    int num_methods;
    StructMethod* methods;
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

#endif // TON_STRUCT_H