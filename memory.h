#ifndef TON_MEMORY_H
#define TON_MEMORY_H

#include <stddef.h>

// Allocation tracking structure
typedef struct Allocation {
    void* ptr;
    size_t size;
    struct Allocation* next;
} Allocation;

// Memory management functions
void* ton_malloc(size_t size);
void* ton_calloc(size_t num, size_t size);
void* ton_realloc(void* ptr, size_t new_size);
void ton_free(void* ptr);
char* ton_strdup(const char* s);

// Memory reporting
void ton_mem_report();

// Memory access functions (unchanged)
int    mem_read_int(void* addr);
void   mem_write_int(void* addr, int value);
char   mem_read_char(void* addr);
void   mem_write_char(void* addr, char value);
float  mem_read_float(void* addr);
void   mem_write_float(void* addr, float value);

void*  ptr_add(void* base, ptrdiff_t bytes);
void*  ptr_sub(void* base, ptrdiff_t bytes);

#endif // TON_MEMORY_H