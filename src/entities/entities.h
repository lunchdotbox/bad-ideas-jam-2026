#ifndef ENTITIES_H
#define ENTITIES_H

#include <elc/core.h>
#include <endian.h>
#include <stdint.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

#include "../physics/particle.h"
#include "../graphics/simple_draw.h"

typedef struct TransformComponent {
    versor rotation;
    vec3 position;
} TransformComponent;

#undef _COMPONENTS_MACRO
#define _COMPONENTS_MACRO\
    X(TRANSFORM, TransformComponent)\
    X(PHYSICS, PhysicsComponent)\
    X(RENDERED, RenderedComponent)

#undef X
#define X(id, data) COMPONENT_##id,
typedef enum Component {
    _COMPONENTS_MACRO
} Component;
#undef X
#define X(id, data) case COMPONENT_##id: return sizeof(data);
ELC_INLINE size_t getComponentSize(Component component) {
    switch (component) {
        _COMPONENTS_MACRO
    }
}
#undef X
#undef _COMPONENTS_MACRO

typedef struct ComponentArchetype {
    Component* components; // TODO: optimize to put component list directly inside bucket data
    void** entities;
    u32 n_components;
    u32 n_entities, max_entities;
} ComponentArchetype;

typedef struct ArchetypeBucket {
    ComponentArchetype* archetypes;
    u32 n_archetypes;
} ArchetypeBucket;

typedef struct EntityMapping {

} EntityMapping;

typedef struct Entities {
    ArchetypeBucket* buckets;
    u32* entities;
    u32 n_buckets, max_buckets;
    u32 n_entities, max_entities;
} Entities;

bool hasCorrectComponents(ComponentArchetype archetype, const Component* components, u32 n_components) {
    if (archetype.n_components != n_components) return false;
    for (u32 i = 0; i < n_components; i++) if (archetype.components[i] != components[i]) return false;
    return true;
}

bool hasCorrectComponentsWithNew(ComponentArchetype archetype, const Component *components, u32 n_components, Component new) {
    if (archetype.n_components != n_components + 1) return false;
    for (u32 i = 0; i < n_components; i++) if (archetype.components[i] != components[i] && archetype.components[i] != new) return false;
    return true;
}

void sortComponents(Component* components, u32 n_components) {
    if (n_components == 0) return;
    while (true) {
        bool should_break = true;
        for (u32 i = 0; i < n_components - 1; i++)
            if (components[i] > components[i + 1]) {
                SWAP(components[i], components[i + 1])
                should_break = false;
            }
        if (should_break) break;
    }
}

void addComponent(Component** components, u32* n_components, Component component, u32 index) {
    (*n_components)++;
    *components = realloc(*components, *n_components * sizeof(Component)); // TODO: possibly optimize to use plain free and malloc depending on index
    for (u32 i = index; i < *n_components - 1; i++) (*components)[i + 1] = (*components)[i];
    (*components)[index] = component;
}

void addComponentInOrder(Component** components, u32* n_components, Component component) {
    for (u32 i = 0; i < *n_components; i++) if ((*components)[i] > component) return addComponent(components, n_components, component, i);
}

u64 hashComponentsWithNew(Component* components, u32 n_components, Component new) {
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
    return hash;
}

u64 hashEntityComponents(Component* components, u32 n_components) {
    u64 hash = ELC_MATH_FNV_OFFSET; // TODO: optimize by making this use a u32 directly
    for (u64 i = 0; i < n_components; i++) {
        hash ^= (u64)(u32)components[i];
        hash *= ELC_MATH_FNV_PRIME;
    }
    return hash;
}

ComponentArchetype createArchetype(Component* components, u32 n_components) {
    ComponentArchetype archetype = {0};
    archetype.n_components = n_components;
    archetype.components = malloc(archetype.n_components * sizeof(Component));
    memcpy(archetype.components, components, n_components * sizeof(Component));
    return archetype;
}

ComponentArchetype createArchetypeWithNew(Component* components, u32 n_components, Component new) {
    ComponentArchetype archetype = createArchetype(components, n_components);
    addComponentInOrder(&archetype.components, &archetype.n_components, new); // TODO: possibly optimize using a loop and memcpy instead of a memcpy and `addComponentInOrder` call
    return archetype;
}

ComponentArchetype* findNextArchetypeWithNew(Entities* entities, Component* components, u32 n_components, Component new) {
    u64 hash = hashComponentsWithNew(components, n_components, new);
    u32 index = hash % entities->n_buckets;

    ArchetypeBucket* bucket = &entities->buckets[index];

    for (u32 i = 0; i < bucket->n_archetypes; i++)
        if (hasCorrectComponentsWithNew(bucket->archetypes[i], components, n_components, new))
            return &bucket->archetypes[i];

    if (bucket->archetypes == NULL) bucket->archetypes = malloc((bucket->n_archetypes + 1) * sizeof(ComponentArchetype));
    else bucket->archetypes = realloc(bucket->archetypes, (bucket->n_archetypes + 1) * sizeof(ComponentArchetype));
    bucket->archetypes[bucket->n_archetypes++] = createArchetypeWithNew(components, n_components, new);

    return &bucket->archetypes[bucket->n_archetypes - 1];
}

ComponentArchetype* findNextArchetype(Entities* entities, Component* components, u32 n_components) { // TODO: compact the archetype finding functions into fewer functions
    u64 hash = hashEntityComponents(components, n_components);
    u32 index = hash % entities->n_buckets;

    ArchetypeBucket* bucket = &entities->buckets[index];

    for (u32 i = 0; i < bucket->n_archetypes; i++)
        if (hasCorrectComponents(bucket->archetypes[i], components, n_components))
            return &bucket->archetypes[i];

    if (bucket->archetypes == NULL) bucket->archetypes = malloc((bucket->n_archetypes + 1) * sizeof(ComponentArchetype));
    else bucket->archetypes = realloc(bucket->archetypes, (bucket->n_archetypes + 1));
    bucket->archetypes[bucket->n_archetypes++] = createArchetype(components, n_components);

    return &bucket->archetypes[bucket->n_archetypes - 1];
}

void allocateArchetypeEntities(ComponentArchetype* archetype, u32 max_entities) {
    archetype->max_entities = max_entities;
    if (ELC_UNLIKELY(archetype->entities == NULL)) {
        archetype->entities = malloc(archetype->n_components * sizeof(void*));
        for (u32 i = 0; i < archetype->n_components; i++) archetype->entities[i] = malloc(archetype->max_entities * getComponentSize(archetype->components[i]));
        return;
    }
    for (u32 i = 0; i < archetype->n_components; i++) archetype->entities[i] = realloc(archetype->entities[i], archetype->max_entities * getComponentSize(archetype->components[i]));
}

void createEntity(Entities* entities, Component* components, u32 n_components, void** data) {
    ComponentArchetype* archetype = findNextArchetype(entities, components, n_components);

    if (archetype->n_entities + 1 > archetype->max_entities) allocateArchetypeEntities(archetype, (archetype->max_entities + 1) * ELC_MATH_GOLDEN_RATIO);

    for (u32 i = 0; i < n_components; i++) {
        size_t component_size = getComponentSize(components[i]);
        memcpy(archetype->entities[i] + (component_size * archetype->n_entities), data[i], component_size);
    }
    archetype->n_entities++;
}

#endif
