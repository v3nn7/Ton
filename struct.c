#include "struct.h"
#include <stdlib.h>
#include <string.h>

TonStructType* define_struct_type(const char* name, StructField* fields, int num_fields, StructMethod* methods, int num_methods) {
    TonStructType* t = (TonStructType*)malloc(sizeof(TonStructType));
    if (!t) return NULL;
    t->name = name;
    t->num_fields = num_fields;
    t->fields = fields;
    t->num_methods = num_methods;
    t->methods = methods;
    return t;
}

void destroy_struct_type(TonStructType* t) {
    if (!t) return;
    free(t);
}

TonStructInstance* create_struct_instance(const TonStructType* t) {
    if (!t) return NULL;
    TonStructInstance* si = (TonStructInstance*)malloc(sizeof(TonStructInstance));
    if (!si) return NULL;
    si->type = t;
    si->field_values = (Value*)calloc(t->num_fields, sizeof(Value));
    return si;
}

void destroy_struct_instance(TonStructInstance* si) {
    if (!si) return;
    if (si->field_values) { free(si->field_values); }
    free(si);
}

int struct_set_field(TonStructInstance* si, const char* field_name, Value v) {
    if (!si || !si->type) return 0;
    for (int i = 0; i < si->type->num_fields; ++i) {
        if (strcmp(si->type->fields[i].name, field_name) == 0) {
            si->field_values[i] = v;
            return 1;
        }
    }
    return 0;
}

Value struct_get_field(const TonStructInstance* si, const char* field_name) {
    Value out; out.type = VALUE_INT; out.data.int_val = 0;
    if (!si || !si->type) return out;
    for (int i = 0; i < si->type->num_fields; ++i) {
        if (strcmp(si->type->fields[i].name, field_name) == 0) {
            return si->field_values[i];
        }
    }
    return out;
}

// Method support functions
FunctionDeclarationNode* struct_get_method(const TonStructType* t, const char* method_name) {
    if (!t || !method_name) return NULL;
    for (int i = 0; i < t->num_methods; ++i) {
        if (strcmp(t->methods[i].name, method_name) == 0) {
            return t->methods[i].function;
        }
    }
    return NULL;
}

Value struct_call_method(TonStructInstance* si, const char* method_name, Value* args, int num_args) {
    Value out; out.type = VALUE_INT; out.data.int_val = 0;
    if (!si || !si->type) return out;
    
    FunctionDeclarationNode* method = struct_get_method(si->type, method_name);
    if (!method) return out;
    
    // TODO: Implement method call with 'this' context
    // This would require integration with the interpreter
    return out;
}