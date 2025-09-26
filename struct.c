#include "struct.h"
#include <stdlib.h>
#include <string.h>

// Global list of defined struct types
static TonStructType** defined_struct_types = NULL;
static int num_defined_struct_types = 0;

TonStructType* define_struct_type(const char* name, StructField* fields, int num_fields, StructMethod* methods, int num_methods) {
    TonStructType* t = (TonStructType*)malloc(sizeof(TonStructType));
    if (!t) return NULL;
    t->name = name;
    t->parent_name = NULL;
    t->parent = NULL;
    t->num_fields = num_fields;
    t->fields = fields;
    t->num_methods = num_methods;
    t->methods = methods;
    t->constructor = NULL;
    t->destructor = NULL;
    t->vtable = NULL;
    t->vtable_size = 0;
    t->total_size = sizeof(TonStructType*) + (num_fields * sizeof(Value));
    
    // Find constructor and destructor
    for (int i = 0; i < num_methods; i++) {
        if (methods[i].is_constructor) {
            t->constructor = &methods[i];
        }
        if (methods[i].is_destructor) {
            t->destructor = &methods[i];
        }
    }
    
    // Add to global list
    defined_struct_types = realloc(defined_struct_types, (num_defined_struct_types + 1) * sizeof(TonStructType*));
    defined_struct_types[num_defined_struct_types] = t;
    num_defined_struct_types++;
    
    return t;
}

void destroy_struct_type(TonStructType* t) {
    if (!t) return;
    if (t->vtable) {
        free(t->vtable);
    }
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
    
    // TODO: Implement method call with interpreter integration
    return out;
}
int check_field_access(const TonStructType* type, const char* field_name, AccessModifier required_access) {
    if (!type || !field_name) return 0;
    
    for (int i = 0; i < type->num_fields; i++) {
        if (strcmp(type->fields[i].name, field_name) == 0) {
            // For now, allow all access - will be refined later
            return type->fields[i].access <= required_access;
        }
    }
    
    // Check parent class if exists
    if (type->parent) {
        return check_field_access(type->parent, field_name, required_access);
    }
    
    return 0;
}

int check_method_access(const TonStructType* type, const char* method_name, AccessModifier required_access) {
    if (!type || !method_name) return 0;
    
    for (int i = 0; i < type->num_methods; i++) {
        if (strcmp(type->methods[i].name, method_name) == 0) {
            return type->methods[i].access <= required_access;
        }
    }
    
    // Check parent class if exists
    if (type->parent) {
        return check_method_access(type->parent, method_name, required_access);
    }
    
    return 0;
}

StructMethod* find_method_in_hierarchy(const TonStructType* type, const char* method_name) {
    if (!type || !method_name) return NULL;
    
    // First check current class
    for (int i = 0; i < type->num_methods; i++) {
        if (strcmp(type->methods[i].name, method_name) == 0) {
            return &type->methods[i];
        }
    }
    
    // Then check parent class
    if (type->parent) {
        return find_method_in_hierarchy(type->parent, method_name);
    }
    
    return NULL;
}

TonStructInstance* create_struct_with_constructor(const TonStructType* t, Value* args, int num_args) {
    if (!t) return NULL;
    
    TonStructInstance* si = create_struct_instance(t);
    if (!si) return NULL;
    
    // Call constructor if exists
    if (t->constructor) {
        // TODO: Implement constructor call with args
        // This would require integration with the interpreter
    }
    
    return si;
}

void destroy_struct_with_destructor(TonStructInstance* si) {
    if (!si) return;
    
    // Call destructor if exists
    if (si->type && si->type->destructor) {
        // TODO: Implement destructor call
        // This would require integration with the interpreter
    }
    
    destroy_struct_instance(si);
}

// Function to find struct type by name
TonStructType* find_struct_type(const char* name) {
    if (!name) return NULL;
    
    for (int i = 0; i < num_defined_struct_types; i++) {
        if (defined_struct_types[i] && strcmp(defined_struct_types[i]->name, name) == 0) {
            return defined_struct_types[i];
        }
    }
    
    return NULL;
}