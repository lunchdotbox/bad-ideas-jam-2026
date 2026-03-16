#ifndef ENGINE_UTILITIES_VECTOR_H
#define ENGINE_UTILITIES_VECTOR_H

#include "inline.h"

typedef double dvec2[2];

#define VEC2_USE(v) {v[0], v[1]}
#define VEC3_USE(v) {v[0], v[1], v[2]}
#define VEC4_USE(v) {v[0], v[1], v[2], v[3]}

INLINE void dvec2_copy(const dvec2 v, dvec2 dst) {
    dst[0] = v[0], dst[1] = v[1];
}

#endif
