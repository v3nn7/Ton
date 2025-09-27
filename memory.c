#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global allocation tracking
static Allocation* allocation_head = NULL;
static size_t total_allocated = 0;

/**
 * Allocate memory and track the allocation
 * @param size Size in bytes to allocate
 * @return Pointer to allocated memory or NULL on failure
 */
void* ton_malloc(size_t size) {
    if (size == 0) return NULL;
    
    void* ptr = malloc(size);
    if (!ptr) return NULL;

    Allocation* new_alloc = (Allocation*)malloc(sizeof(Allocation));
    if (!new_alloc) {
        free(ptr);
        return NULL;
    }

    new_alloc->ptr = ptr;
    new_alloc->size = size;
    new_alloc->next = allocation_head;
    allocation_head = new_alloc;
    total_allocated += size;

    return ptr;
}

/**
 * Free tracked memory allocation
 * @param ptr Pointer to memory to free
 */
void ton_free(void* ptr) {
    if (!ptr) return;

    Allocation** current = &allocation_head;
    while (*current) {
        if ((*current)->ptr == ptr) {
            Allocation* to_free = *current;
            *current = to_free->next;
            total_allocated -= to_free->size;
            free(to_free->ptr);
            free(to_free);
            return;
        }
        current = &(*current)->next;
    }
    
    // If we reach here, the pointer wasn't tracked - free it anyway
    free(ptr);
}

void* ton_calloc(size_t num, size_t size) {
    size_t total_size = num * size;
    void* ptr = ton_malloc(total_size);
    if (ptr) {
        memset(ptr, 0, total_size);
    }
    return ptr;
}

void* ton_realloc(void* ptr, size_t new_size) {
    if (!ptr) return ton_malloc(new_size);

    Allocation** current = &allocation_head;
    while (*current) {
        if ((*current)->ptr == ptr) {
            void* new_ptr = realloc(ptr, new_size);
            if (!new_ptr) return NULL; // Realloc failed

            total_allocated -= (*current)->size;
            total_allocated += new_size;
            (*current)->ptr = new_ptr;
            (*current)->size = new_size;
            return new_ptr;
        }
        current = &(*current)->next;
    }

    return NULL; // Should not happen if ptr is valid
}

char* ton_strdup(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char* new_str = (char*)ton_malloc(len);
    if (!new_str) return NULL;
    memcpy(new_str, s, len);
    return new_str;
}

void ton_mem_report() {
    printf("--- Memory Report ---\n");
    printf("Total allocated: %llu bytes\n", (unsigned long long)total_allocated);
    printf("Active allocations:\n");

    Allocation* current = allocation_head;
    int count = 0;
    while (current) {
        printf("  - %p: %llu bytes\n", current->ptr, (unsigned long long)current->size);
        current = current->next;
        count++;
    }
    printf("Total active allocations: %d\n", count);
    printf("---------------------\n");
}

// Unchanged memory access functions
int    mem_read_int(void* addr) { return *(int*)addr; }
void   mem_write_int(void* addr, int value) { *(int*)addr = value; }
char   mem_read_char(void* addr) { return *(char*)addr; }
void   mem_write_char(void* addr, char value) { *(char*)addr = value; }
float  mem_read_float(void* addr) { return *(float*)addr; }
void   mem_write_float(void* addr, float value) { *(float*)addr = value; }

void*  ptr_add(void* base, ptrdiff_t bytes) { return (void*)((char*)base + bytes); }
void*  ptr_sub(void* base, ptrdiff_t bytes) { return (void*)((char*)base - bytes); }

/**
 * Clean up all remaining allocations (for program shutdown)
 */
void ton_mem_cleanup() {
    while (allocation_head) {
        Allocation* to_free = allocation_head;
        allocation_head = allocation_head->next;
        free(to_free->ptr);
        free(to_free);
    }
    total_allocated = 0;
}

/**
 * Get current memory usage statistics
 * @return Total bytes currently allocated
 */
size_t ton_mem_usage() {
    return total_allocated;
}