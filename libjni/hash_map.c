
#include <string.h>
#include <stdlib.h>

#include "hash_map.h"

static unsigned elfhash(const char *_name) {
    const unsigned char *name = (const unsigned char *) _name;
    unsigned h = 0, g;

    while(*name) {
        h = (h << 4) + *name++;
        g = h & 0xf0000000;
        h ^= g;
        h ^= g >> 24;
    }
    return h;
}

void HashMap_init(HashMap* map) {
    for (int i = 0; i < HASH_BUCKET_NUM; i++) {
        map->buckets[i].key = NULL;
        map->buckets[i].value = NULL;
        map->buckets[i].next = NULL;
    }
}

void* HashMap_get(HashMap* map, const char* key) {
    unsigned hash = elfhash(key);
    MapEntry* bucket = &map->buckets[hash % HASH_BUCKET_NUM];
    for (MapEntry* e = bucket->next; e != NULL; ) {
        if (strcmp(e->key, key) == 0) {
            return e->value;
        }
        e = e->next;
    }
    return NULL;
}

void HashMap_put(HashMap* map, const char* key, void* value) {
    unsigned hash = elfhash(key);
    MapEntry* bucket = &map->buckets[hash % HASH_BUCKET_NUM];
    if (bucket->next == NULL) {
        MapEntry* e = malloc(sizeof(MapEntry));
        e->key = strdup(key);
        e->value = value;
        e->next = NULL;
        bucket->next = e;
        return;
    }
    for (MapEntry* e = bucket->next; e != NULL; ) {
        if (strcmp(e->key, key) == 0) {
            e->value = value;
            return;
        }
        e = e->next;
    }
    MapEntry* next = bucket->next;
    MapEntry* e = malloc(sizeof(MapEntry));
    e->key = strdup(key);
    e->value = value;
    e->next = next;
    bucket->next = e;
    return;
}

void HashMap_remove(HashMap* map, const char* key) {
    unsigned hash = elfhash(key);
    MapEntry* bucket = &map->buckets[hash % HASH_BUCKET_NUM];
    MapEntry* last = bucket;
    for (MapEntry* e = bucket->next; e != NULL; ) {
        if (strcmp(e->key, key) == 0) {
            last->next = e->next;
            free(e->key);
            free(e);
            return;
        }
        last = e;
        e = e->next;
    }
}

