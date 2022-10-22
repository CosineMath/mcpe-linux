#ifndef HASH_MAP_H
#define HASH_MAP_H

#define HASH_BUCKET_NUM 5

struct _MapEntry {
    char* key;
    void* value;
    struct _MapEntry* next;
};

typedef struct _MapEntry MapEntry;
struct _HashMap {
    MapEntry buckets[HASH_BUCKET_NUM];
};
typedef struct _HashMap HashMap;

void HashMap_init(HashMap* map);
void* HashMap_get(HashMap* map, const char* key);
void HashMap_put(HashMap* map, const char* key, void* value);
void HashMap_remove(HashMap* map, const char* key);

#endif