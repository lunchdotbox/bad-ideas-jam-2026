#ifndef ENGINE_GRAPHICS_POINT_LIGHTS_H
#define ENGINE_GRAPHICS_POINT_LIGHTS_H

#include "color.h"

#include <cglm/cglm.h>

typedef struct PointLight {
    vec3 position;
    u8 r, g, b;
} PointLight;

#endif
