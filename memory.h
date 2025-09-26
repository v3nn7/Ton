#ifndef TON_MEMORY_H
#define TON_MEMORY_H

#include <stddef.h>

void* ton_malloc(size_t size);
void  ton_free(void* ptr);

int    mem_read_int(void* addr);
void   mem_write_int(void* addr, int value);
char   mem_read_char(void* addr);
void   mem_write_char(void* addr, char value);

void*  ptr_add(void* base, ptrdiff_t bytes);
void*  ptr_sub(void* base, ptrdiff_t bytes);

#endif // TON_MEMORY_H