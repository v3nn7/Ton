#ifndef COLLECTIONS_H
#define COLLECTIONS_H

#include "value.h"

#define TONLIST_INITIAL_CAPACITY 8
#define TONMAP_INITIAL_CAPACITY 16

// TonList - Dynamic array of Values
typedef struct {
    Value* data;
    int size;
    int capacity;
} TonList;

// TonMap - Hash map with string keys and Value values
typedef struct TonMapEntry {
    char* key;
    Value value;
    struct TonMapEntry* next;
} TonMapEntry;

typedef struct {
    TonMapEntry** buckets;
    int capacity;
    int size;
} TonMap;

// TonSet - Set of strings (using TonMap internally)
typedef struct {
    TonMap* map;
} TonSet;

// TonList functions
TonList* tonlist_create();
void tonlist_destroy(TonList* list);
int tonlist_push(TonList* list, Value value);
Value tonlist_pop(TonList* list);
Value tonlist_get(TonList* list, int index);
int tonlist_set(TonList* list, int index, Value value);
int tonlist_size(TonList* list);

// TonMap functions
TonMap* tonmap_create();
void tonmap_destroy(TonMap* map);
int tonmap_set(TonMap* map, const char* key, Value value);
Value tonmap_get(TonMap* map, const char* key);
int tonmap_has(TonMap* map, const char* key);
int tonmap_remove(TonMap* map, const char* key);
int tonmap_size(TonMap* map);

// TonSet functions
TonSet* tonset_create();
void tonset_destroy(TonSet* set);
int tonset_add(TonSet* set, const char* value);
int tonset_has(TonSet* set, const char* value);
int tonset_remove(TonSet* set, const char* value);
int tonset_size(TonSet* set);

#endif // COLLECTIONS_H