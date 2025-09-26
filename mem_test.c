#include "memory.h"
#include <stdio.h>

int main() {
    printf("--- Running Memory Test ---\n");

    // Test 1: Simple allocation and deallocation
    printf("Test 1: Simple allocation and deallocation\n");
    void* block1 = ton_malloc(100);
    ton_mem_report();
    ton_free(block1);
    ton_mem_report();

    // Test 2: Multiple allocations
    printf("\nTest 2: Multiple allocations\n");
    void* block2 = ton_malloc(50);
    void* block3 = ton_malloc(75);
    ton_mem_report();
    ton_free(block2);
    ton_mem_report();
    ton_free(block3);
    ton_mem_report();

    // Test 3: Calloc
    printf("\nTest 3: Calloc\n");
    int* arr1 = (int*)ton_calloc(10, sizeof(int));
    ton_mem_report();
    ton_free(arr1);
    ton_mem_report();

    // Test 4: Realloc
    printf("\nTest 4: Realloc\n");
    char* str1 = (char*)ton_malloc(10);
    ton_mem_report();
    str1 = (char*)ton_realloc(str1, 20);
    ton_mem_report();
    ton_free(str1);
    ton_mem_report();

    // Test 5: Strdup
    printf("\nTest 5: Strdup\n");
    const char* original = "Hello, Ton-Lang!";
    char* duplicated = ton_strdup(original);
    ton_mem_report();
    ton_free(duplicated);
    ton_mem_report();

    printf("--- Memory Test Finished ---\n");

    return 0;
}