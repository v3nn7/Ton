#include "memory.h"
#include <stdlib.h>

void* ton_malloc(size_t size) { return malloc(size); }
void  ton_free(void* ptr) { free(ptr); }

int    mem_read_int(void* addr) { return *(int*)addr; }
void   mem_write_int(void* addr, int value) { *(int*)addr = value; }
char   mem_read_char(void* addr) { return *(char*)addr; }
void   mem_write_char(void* addr, char value) { *(char*)addr = value; }

void*  ptr_add(void* base, ptrdiff_t bytes) { return (void*)((char*)base + bytes); }
void*  ptr_sub(void* base, ptrdiff_t bytes) { return (void*)((char*)base - bytes); }