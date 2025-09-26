#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_value(const Value* v) {
    if (!v) return;
    switch (v->type) {
        case VALUE_INT:
            printf("%d\n", v->data.int_val);
            break;
        case VALUE_STRING:
            printf("%s\n", v->data.string_val ? v->data.string_val : "");
            break;
        default:
            printf("(unknown value type)\n");
            break;
    }
}

Value read_line_value() {
    size_t cap = 128, len = 0;
    char* buf = (char*)malloc(cap);
    if (!buf) {
        return create_value_string("");
    }
    int ch;
    while ((ch = fgetc(stdin)) != EOF) {
        if (ch == '\n') break;
        if (len + 1 >= cap) {
            size_t new_cap = cap * 2;
            char* t = (char*)realloc(buf, new_cap);
            if (!t) { free(buf); return create_value_string(""); }
            buf = t; cap = new_cap;
        }
        buf[len++] = (char)ch;
    }
    if (len > 0 && buf[len-1] == '\r') len--; // trim Windows CR
    buf[len] = '\0';
    Value v = create_value_string(buf);
    free(buf);
    return v;
}