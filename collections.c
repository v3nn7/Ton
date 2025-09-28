#include "collections.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TONMAP_BUCKET_COUNT 16

// TonList implementation
TonList* tonlist_create() {
    TonList* list = malloc(sizeof(TonList));
    if (!list) return NULL;
    
    list->data = malloc(sizeof(Value) * TONLIST_INITIAL_CAPACITY);
    if (!list->data) {
        free(list);
        return NULL;
    }
    
    list->size = 0;
    list->capacity = TONLIST_INITIAL_CAPACITY;
    return list;
}

void tonlist_destroy(TonList* list) {
    if (!list) return;
    free(list->data);
    free(list);
}

int tonlist_push(TonList* list, Value value) {
    if (!list) return 0;
    
    if (list->size >= list->capacity) {
        int new_capacity = list->capacity * 2;
        Value* new_data = realloc(list->data, sizeof(Value) * new_capacity);
        if (!new_data) return 0;
        
        list->data = new_data;
        list->capacity = new_capacity;
    }
    
    list->data[list->size++] = value;
    return 1;
}

Value tonlist_pop(TonList* list) {
    if (!list || list->size == 0) {
        return create_value_null();
    }
    
    return list->data[--list->size];
}

Value tonlist_get(TonList* list, int index) {
    if (!list || index < 0 || index >= list->size) {
        return create_value_null();
    }
    
    return list->data[index];
}

int tonlist_set(TonList* list, int index, Value value) {
    if (!list || index < 0 || index >= list->size) {
        return 0;
    }
    
    list->data[index] = value;
    return 1;
}

int tonlist_size(TonList* list) {
    return list ? list->size : 0;
}

// TonMap implementation (simple hash table)
static unsigned int hash_string(const char* str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

TonMap* tonmap_create() {
    TonMap* map = malloc(sizeof(TonMap));
    if (!map) return NULL;
    
    map->buckets = calloc(TONMAP_BUCKET_COUNT, sizeof(TonMapEntry*));
    if (!map->buckets) {
        free(map);
        return NULL;
    }
    
    map->size = 0;
    return map;
}

void tonmap_destroy(TonMap* map) {
    if (!map) return;
    
    for (int i = 0; i < TONMAP_BUCKET_COUNT; i++) {
        TonMapEntry* entry = map->buckets[i];
        while (entry) {
            TonMapEntry* next = entry->next;
            free(entry->key);
            free(entry);
            entry = next;
        }
    }
    
    free(map->buckets);
    free(map);
}

int tonmap_set(TonMap* map, const char* key, Value value) {
    if (!map || !key) return 0;
    
    unsigned int bucket = hash_string(key) % TONMAP_BUCKET_COUNT;
    TonMapEntry* entry = map->buckets[bucket];
    
    // Check if key already exists
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            entry->value = value;
            return 1;
        }
        entry = entry->next;
    }
    
    // Create new entry
    TonMapEntry* new_entry = malloc(sizeof(TonMapEntry));
    if (!new_entry) return 0;
    
    new_entry->key = strdup(key);
    if (!new_entry->key) {
        free(new_entry);
        return 0;
    }
    
    new_entry->value = value;
    new_entry->next = map->buckets[bucket];
    map->buckets[bucket] = new_entry;
    map->size++;
    
    return 1;
}

Value tonmap_get(TonMap* map, const char* key) {
    if (!map || !key) return create_value_null();
    
    unsigned int bucket = hash_string(key) % TONMAP_BUCKET_COUNT;
    TonMapEntry* entry = map->buckets[bucket];
    
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    
    return create_value_null();
}

int tonmap_has(TonMap* map, const char* key) {
    if (!map || !key) return 0;
    
    unsigned int bucket = hash_string(key) % TONMAP_BUCKET_COUNT;
    TonMapEntry* entry = map->buckets[bucket];
    
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return 1;
        }
        entry = entry->next;
    }
    
    return 0;
}

int tonmap_remove(TonMap* map, const char* key) {
    if (!map || !key) return 0;
    
    unsigned int bucket = hash_string(key) % TONMAP_BUCKET_COUNT;
    TonMapEntry* entry = map->buckets[bucket];
    TonMapEntry* prev = NULL;
    
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                map->buckets[bucket] = entry->next;
            }
            
            free(entry->key);
            free(entry);
            map->size--;
            return 1;
        }
        prev = entry;
        entry = entry->next;
    }
    
    return 0;
}

int tonmap_size(TonMap* map) {
    return map ? map->size : 0;
}

// TonSet implementation (using TonMap internally)
TonSet* tonset_create() {
    TonSet* set = malloc(sizeof(TonSet));
    if (!set) return NULL;
    
    set->map = tonmap_create();
    if (!set->map) {
        free(set);
        return NULL;
    }
    
    return set;
}

void tonset_destroy(TonSet* set) {
    if (!set) return;
    tonmap_destroy(set->map);
    free(set);
}

int tonset_add(TonSet* set, const char* value) {
    if (!set || !value) return 0;
    return tonmap_set(set->map, value, create_value_bool(true));
}

int tonset_has(TonSet* set, const char* value) {
    if (!set || !value) return 0;
    return tonmap_has(set->map, value);
}

int tonset_remove(TonSet* set, const char* value) {
    if (!set || !value) return 0;
    return tonmap_remove(set->map, value);
}

int tonset_size(TonSet* set) {
    return set ? tonmap_size(set->map) : 0;
}