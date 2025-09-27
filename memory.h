#ifndef TON_MEMORY_H
#define TON_MEMORY_H

#include <stddef.h>

/**
 * Allocation tracking structure for memory management
 */
typedef struct Allocation {
    void* ptr;                    // Pointer to allocated memory
    size_t size;                  // Size of allocation in bytes
    struct Allocation* next;      // Next allocation in linked list
} Allocation;

// Core memory management functions
void* ton_malloc(size_t size);
void* ton_calloc(size_t num, size_t size);
void* ton_realloc(void* ptr, size_t new_size);
void ton_free(void* ptr);
char* ton_strdup(const char* s);

// Memory tracking and cleanup functions
void ton_mem_report();
void ton_mem_cleanup();
size_t ton_mem_usage();

// Low-level memory access functions
int    mem_read_int(void* addr);
void   mem_write_int(void* addr, int value);
char   mem_read_char(void* addr);
void   mem_write_char(void* addr, char value);
float  mem_read_float(void* addr);
void   mem_write_float(void* addr, float value);

// Pointer arithmetic helpers
void*  ptr_add(void* base, ptrdiff_t bytes);
void*  ptr_sub(void* base, ptrdiff_t bytes);

#endif // TON_MEMORY_H