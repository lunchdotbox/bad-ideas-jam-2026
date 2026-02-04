#include "entities.h"

#include <elc/core.h>
#include <stdio.h>
#include <stdlib.h>

#define INITIAL_ARCHETYPE_MAX_ENTITIES 128
#define INITIAL_ENTITIES_MAX_ENTITIES 1024
#define INITIAL_ENTITIES_BUCKETS 128
#define ENTITIES_MAX_ARCHETYPES_PER_BUCKET 1
#define INITIAL_BUCKET_LIST_MAX 16

typedef struct EntityListsStart {
    u32 n_components;
    u32 n_entities;
    u32 max_entities;
    u32* entity_ids;
} EntityListsStart;

typedef struct ArchetypeBucket {
    u64 component_hashes[ENTITIES_MAX_ARCHETYPES_PER_BUCKET];
    void* archetype_data[ENTITIES_MAX_ARCHETYPES_PER_BUCKET];
    u64* component_hash_list;
    void** archetype_data_list;
    u32 n_archetypes, max_archetyps;
} ArchetypeBucket;

typedef struct EntityMapping {
    void* archetype_data;
    u32 data_index;
} EntityMapping;

typedef struct Archetype {
    void* data;
    u64 hash;
} Archetype;

Entities createEntitySystem() {
    Entities entities = {.max_entities = INITIAL_ENTITIES_MAX_ENTITIES, .n_buckets = INITIAL_ENTITIES_BUCKETS};
    entities.entities = malloc(entities.max_entities * sizeof(EntityMapping));
    entities.buckets = calloc(entities.n_buckets, sizeof(ArchetypeBucket));
    return entities;
}

ELC_INLINE Archetype getBucketArchetype(ArchetypeBucket* bucket, u32 index) {
    if (index < ENTITIES_MAX_ARCHETYPES_PER_BUCKET)
        return (Archetype){.data = bucket->archetype_data[index], .hash = bucket->component_hashes[index]};
    else {
        u32 list_index = index - ENTITIES_MAX_ARCHETYPES_PER_BUCKET;
        return (Archetype){.data = bucket->archetype_data_list[list_index], .hash = bucket->component_hash_list[list_index]};
    }
}

void destroyEntitySystem(Entities entities) {
    for (u32 i = 0; i < entities.n_buckets; i++) {
        ArchetypeBucket* bucket = &entities.buckets[i];
        for (u32 j = 0; j < bucket->n_archetypes; j++) {
            Archetype archetype = getBucketArchetype(bucket, j);
            EntityListsStart* start = archetype.data;
            void** entity_list = archetype.data + sizeof(EntityListsStart) + (start->n_components * sizeof(Component));
            for (u32 k = 0; k < start->n_components; k++) free(entity_list[k]);
            free(start->entity_ids), free(archetype.data);
        }
        if (bucket->archetype_data_list != NULL) free(bucket->archetype_data_list), free(bucket->component_hash_list);
    }

    free(entities.buckets), free(entities.entities);
}

ELC_INLINE bool hasCorrectComponents(const Component* components_a, u32 n_components_a, const Component* components_b, u32 n_components_b) {
    if (n_components_a != n_components_b) return false;
    for (u32 i = 0; i < n_components_a; i++) if (components_a[i] != components_b[i]) return false;
    return true;
}

ELC_INLINE bool hasCorrectComponentsWithNew(const Component *components_a, u32 n_components_a, const Component *components_b, u32 n_components_b, Component new) {
    if (n_components_a != n_components_b + 1) return false;
    for (u32 i = 0; i < n_components_b; i++) if (components_a[i] != components_b[i] && components_a[i] != new) return false;
    return true;
}

ELC_INLINE void sortComponents(Component* components, u32 n_components, void** data) {
    if (n_components == 0) return;
    while (true) {
        bool should_break = true;
        for (u32 i = 0; i < n_components - 1; i++)
            if (components[i] > components[i + 1]) {
                SWAP(components[i], components[i + 1])
                if (data != NULL) SWAP(data[i], data[i + 1]);
                should_break = false;
            }
        if (should_break) break;
    }
}

ELC_INLINE u64 hashComponentsWithNew(Component* components, u32 n_components, Component new) {
    u64 hash = ELC_MATH_FNV_OFFSET; // TODO: optimize by making this use a u32 directly
    bool has_hashed_new = false;
    for (u64 i = 0; i < n_components; i++) {
        if (components[i] > new && !has_hashed_new) {
            hash ^= (u64)(u32)new;
            hash *= ELC_MATH_FNV_PRIME;
            has_hashed_new = true;
        }
        hash ^= (u64)(u32)components[i];
        hash *= ELC_MATH_FNV_PRIME;
    }
    if (!has_hashed_new) {
        hash ^= (u64)(u32)new;
        hash *= ELC_MATH_FNV_PRIME;
    }
    return hash;
}

ELC_INLINE u64 hashEntityComponents(Component* components, u32 n_components) {
    u64 hash = ELC_MATH_FNV_OFFSET; // TODO: optimize by making this use a u32 directly
    for (u64 i = 0; i < n_components; i++) {
        hash ^= (u64)(u32)components[i];
        hash *= ELC_MATH_FNV_PRIME;
    }
    return hash;
}

ELC_INLINE void createNewComponentList(Component* components, u32 n_components, Component new, Component* memory) {
    for (u32 i = 0; i <= n_components; i++) {
        if (i >= n_components) {
            memory[i] = new;
            break;
        }
        if (components[i] > new) {
            memory[i] = new;
            memcpy(&memory[i + 1], &components[i], ((n_components - 1) - i) * sizeof(Component));
            break;
        }
        memory[i] = components[i];
    }
}

ELC_INLINE void addArchetype(ArchetypeBucket* bucket, void* data, u64 hash) {
    if (bucket->n_archetypes < ENTITIES_MAX_ARCHETYPES_PER_BUCKET) {
        bucket->archetype_data[bucket->n_archetypes] = data;
        bucket->component_hashes[bucket->n_archetypes++] = hash;
    } else {
        if (bucket->archetype_data_list == NULL) {
            bucket->max_archetyps = INITIAL_BUCKET_LIST_MAX;
            bucket->archetype_data_list = malloc(bucket->max_archetyps * sizeof(void*));
            bucket->component_hash_list = malloc(bucket->max_archetyps * sizeof(u64));
        } else {
            if ((bucket->n_archetypes - INITIAL_BUCKET_LIST_MAX) + 1 > bucket->max_archetyps) {
                bucket->max_archetyps *= ELC_MATH_GOLDEN_RATIO;
                bucket->archetype_data_list = realloc(bucket->archetype_data_list, bucket->max_archetyps * sizeof(void*));
                bucket->component_hash_list = realloc(bucket->component_hash_list, bucket->max_archetyps * sizeof(u64));
            }
        }
        bucket->archetype_data_list[bucket->n_archetypes - INITIAL_BUCKET_LIST_MAX] = data;
        bucket->component_hash_list[bucket->n_archetypes++ - INITIAL_BUCKET_LIST_MAX] = hash;
    }
}

ELC_INLINE void freeArchetypeMap(ArchetypeBucket* buckets, u32 n_buckets) {
    for (u32 i = 0; i < n_buckets; i++) if (buckets->archetype_data_list != NULL) free(buckets->archetype_data_list), free(buckets->component_hash_list);

    free(buckets);
}

ELC_INLINE void resizeArchetypeMap(Entities* entities, u32 n_buckets) {
    ArchetypeBucket* new_buckets = calloc(n_buckets, sizeof(ArchetypeBucket));

    for (u32 i = 0; i < entities->n_buckets; i++) {
        ArchetypeBucket* src = &entities->buckets[i];
        for (u32 j = 0; j < src->n_archetypes; j++) {
            Archetype archetype = getBucketArchetype(src, i);
            u32 index = archetype.hash % n_buckets;

            addArchetype(&new_buckets[index], archetype.data, archetype.hash);
        }
    }

    freeArchetypeMap(entities->buckets, entities->n_buckets);

    entities->buckets = new_buckets;
    entities->n_buckets = n_buckets;
}

ELC_INLINE void* createArchetype(Component* components, u32 n_components, u32 max_entities) {
    void* result = malloc(sizeof(EntityListsStart) + (n_components * sizeof(Component)) + (n_components * sizeof(void*)));
    void** entity_list = result + sizeof(EntityListsStart) + (n_components * sizeof(Component));
    *(EntityListsStart*)result = (EntityListsStart){.n_components = n_components, .max_entities = max_entities, .entity_ids = malloc(max_entities * sizeof(u32))};
    for (u32 i = 0; i < n_components; i++) entity_list[i] = malloc(max_entities * getComponentSize(components[i]));
    memcpy(result + sizeof(EntityListsStart), components, n_components * sizeof(Component));

    return result;
}

ELC_INLINE void* createArchetypeWithNew(Component* components, u32 n_components, u32 max_entities, Component new) {
    void* result = malloc(sizeof(EntityListsStart) + ((n_components + 1) * sizeof(Component)) + ((n_components + 1) * sizeof(void*)));
    void** entity_list = result + sizeof(EntityListsStart) + ((n_components + 1) * sizeof(Component));
    *(EntityListsStart*)result = (EntityListsStart){.n_components = n_components + 1, .max_entities = max_entities, .entity_ids = malloc(max_entities * sizeof(u32))};
    for (u32 i = 0; i <= n_components; i++) {
        if (i >= n_components) entity_list[i] = malloc(max_entities * getComponentSize(new));
        else if (components[i] > new) {
            entity_list[i] = malloc(max_entities * getComponentSize(new));
            for (u32 j = 0; j < (n_components - 1) - i; j++) entity_list[i + j + 1] = malloc(max_entities * getComponentSize(components[i + j]));
            break;
        } else entity_list[i] = malloc(max_entities * getComponentSize(components[i]));
    }
    createNewComponentList(components, n_components, new, result + sizeof(EntityListsStart));
    return result;
}

ELC_INLINE void* findNextArchetypeWithNew(Entities* entities, Component* components, u32 n_components, Component new) {
    u64 hash = hashComponentsWithNew(components, n_components, new);
    u32 index = hash % entities->n_buckets;

    ArchetypeBucket* bucket = &entities->buckets[index];

    for (u32 i = 0; i < bucket->n_archetypes; i++) {
        Archetype archetype = getBucketArchetype(bucket, i);
        EntityListsStart* start = archetype.data;
        if (archetype.hash == hash && hasCorrectComponentsWithNew(archetype.data + sizeof(EntityListsStart) + (start->n_components * sizeof(Component)), start->n_components, components, n_components, new))
            return archetype.data;
    }

    void* archetype = createArchetypeWithNew(components, n_components, INITIAL_ARCHETYPE_MAX_ENTITIES, new);
    addArchetype(bucket, archetype, hash);

    return archetype;
}

ELC_INLINE void* findNextArchetype(Entities* entities, Component* components, u32 n_components) {
    u64 hash = hashEntityComponents(components, n_components);
    u32 index = hash % entities->n_buckets;

    ArchetypeBucket* bucket = &entities->buckets[index];

    for (u32 i = 0; i < bucket->n_archetypes; i++) {
        Archetype archetype = getBucketArchetype(bucket, i);
        EntityListsStart* start = archetype.data;
        if (archetype.hash == hash && hasCorrectComponents(archetype.data + sizeof(EntityListsStart) + (start->n_components * sizeof(Component)), start->n_components, components, n_components))
            return archetype.data;
    }

    void* archetype = createArchetype(components, n_components, INITIAL_ARCHETYPE_MAX_ENTITIES);
    addArchetype(bucket, archetype, hash);

    return archetype;
}

ELC_INLINE void allocateArchetypeEntities(void* archetype, u32 max_entities) {
    EntityListsStart* start = archetype;
    void** entity_list = archetype + sizeof(EntityListsStart) + (start->n_components * sizeof(Component));
    Component* component_list = archetype + sizeof(EntityListsStart);
    start->max_entities = max_entities;
    for (u32 i = 0; i < start->n_components; i++) entity_list[i] = realloc(entity_list[i], max_entities * getComponentSize(component_list[i]));
    start->entity_ids = realloc(start->entity_ids, start->max_entities * sizeof(u32));
}

ELC_INLINE void allocateEntityMappings(Entities* entities, u32 max_entities) {
    entities->max_entities = max_entities;
    entities->entities = realloc(entities->entities, entities->max_entities * sizeof(EntityMapping));
}

ELC_INLINE u32 addEntityMapping(Entities* entities, EntityMapping mapping) {
    if (entities->n_entities + 1 > entities->max_entities) allocateEntityMappings(entities, entities->max_entities * ELC_MATH_GOLDEN_RATIO);
    entities->entities[entities->n_entities++] = mapping;
    return entities->n_entities - 1;
}

u32 createEntityUnsorted(Entities* entities, Component* components, u32 n_components, void** data) {
    void* archetype = findNextArchetype(entities, components, n_components);

    EntityListsStart* start = archetype;
    void** entity_list = archetype + sizeof(EntityListsStart) + (start->n_components * sizeof(Component));
    Component* component_list = archetype + sizeof(EntityListsStart);

    if (start->n_entities + 1 > start->max_entities) allocateArchetypeEntities(archetype, start->max_entities * ELC_MATH_GOLDEN_RATIO);

    if (data != NULL) for (u32 i = 0; i < n_components; i++) if (data[i] != NULL) {
        size_t component_size = getComponentSize(component_list[i]);
        memcpy(entity_list[i] + (start->n_entities * component_size), data[i], component_size);
    }

    start->entity_ids[start->n_entities] = entities->n_entities;

    EntityMapping mapping = {
        .data_index = start->n_entities++,
        .archetype_data = archetype,
    };

    return addEntityMapping(entities, mapping);
}

u32 createEntity(Entities* entities, Component* components, u32 n_components, void** data) {
    sortComponents(components, n_components, data);
    return createEntityUnsorted(entities, components, n_components, data);
}

void destroyEntity(Entities* entities, u32 id) {
    EntityMapping* mapping = &entities->entities[id];
    EntityListsStart* start = mapping->archetype_data;
    void** entity_list = mapping->archetype_data + sizeof(EntityListsStart) + (start->n_components * sizeof(Component));
    Component* component_list = mapping->archetype_data + sizeof(EntityListsStart);

    if (mapping->data_index != start->n_entities - 1) {
        u32 swap_id = start->entity_ids[start->n_entities - 1];
        EntityMapping* swap = &entities->entities[swap_id];

        for (u32 i = 0; i < start->n_components; i++) {
            size_t component_size = getComponentSize(component_list[i]);
            memcpy(entity_list[i] + (mapping->data_index * component_size), entity_list[i] + (swap->data_index * component_size), component_size);
        }

        swap->data_index = mapping->data_index;
    }

    if (start->n_entities - 1 < start->max_entities / ELC_MATH_GOLDEN_RATIO) allocateArchetypeEntities(mapping->archetype_data, MAX(start->max_entities / ELC_MATH_GOLDEN_RATIO, INITIAL_ARCHETYPE_MAX_ENTITIES));
    start->n_entities--;

    if (id == entities->n_entities - 1) {
        entities->n_entities--;
        if (entities->n_entities > 0) for (u32 i = entities->n_entities - 1; i >= 0 && entities->entities[i].archetype_data == NULL; i--) entities->n_entities--;
    } else entities->entities[id].archetype_data = NULL;

    if (entities->n_entities < entities->max_entities / ELC_MATH_GOLDEN_RATIO) allocateEntityMappings(entities, MAX(entities->max_entities / ELC_MATH_GOLDEN_RATIO, INITIAL_ENTITIES_MAX_ENTITIES));
}

void addComponent(Entities* entities, u32 id, Component new, const void* data) {
    EntityMapping* mapping = &entities->entities[id];
    Component* components = mapping->archetype_data + sizeof(EntityListsStart);
    EntityListsStart* start = mapping->archetype_data;
    void** entity_list = mapping->archetype_data + sizeof(EntityListsStart) + (start->n_components * sizeof(Component));

    void* archetype = findNextArchetypeWithNew(entities, components, start->n_components, new);

    EntityListsStart* new_start = archetype;
    void** new_entity_list = archetype + sizeof(EntityListsStart) + (new_start->n_components * sizeof(Component));
    Component* new_components = archetype + sizeof(EntityListsStart);

    if (new_start->n_entities + 1 > new_start->max_entities) allocateArchetypeEntities(archetype, new_start->max_entities * ELC_MATH_GOLDEN_RATIO);

    for (u32 i = 0, index = 0; i < start->n_components; i++, index++) { // possible bug here
        if (new_components[i] == new) {
            if (data != NULL) {
                size_t component_size = getComponentSize(new);
                memcpy(new_entity_list[index] + (new_start->n_entities * component_size), data, component_size);
            }
            i--;
            continue;
        }

        size_t component_size = getComponentSize(components[i]);
        memcpy(new_entity_list[index] + (new_start->n_entities * component_size), entity_list[i] + (mapping->data_index * component_size), component_size);
    }

    if (mapping->data_index != start->n_entities - 1) {
        u32 swap_id = start->entity_ids[start->n_entities - 1];
        EntityMapping* swap = &entities->entities[swap_id];

        for (u32 i = 0; i < start->n_components; i++) {
            size_t component_size = getComponentSize(components[i]);
            memcpy(entity_list[i] + (mapping->data_index * component_size), entity_list[i] + (swap->data_index * component_size), component_size);
        }

        swap->data_index = mapping->data_index;
    }

    if (start->n_entities - 1 < start->max_entities / ELC_MATH_GOLDEN_RATIO) allocateArchetypeEntities(mapping->archetype_data, MAX(start->max_entities / ELC_MATH_GOLDEN_RATIO, INITIAL_ARCHETYPE_MAX_ENTITIES));
    start->n_entities--;

    mapping->data_index = new_start->n_entities++;
    mapping->archetype_data = archetype;
}
