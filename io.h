#ifndef TON_IO_H
#define TON_IO_H

#include "interpreter.h"

// Print a Value using Ton's basic I/O rules (supports int and string for now)
void print_value(const Value* v);

// Read a line from stdin and return as VALUE_STRING (owns_data=1)
Value read_line_value();

#endif // TON_IO_H