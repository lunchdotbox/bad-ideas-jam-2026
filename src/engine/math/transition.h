#ifndef ENGINE_MATH_TRANSITION_H
#define ENGINE_MATH_TRANSITION_H

#include "../utilities/inline.h"

#include <cglm/cglm.h>
#include <math.h>

INLINE double easeInCubic(double t) {
    return t * t * t;
}

INLINE double easeInQuad(double t) {
    return t * t;
}

INLINE double easeOutQuad(double t) {
    return -1.0 * t * (t - 2.0);
}

INLINE double easeOutElastic(double t) {
    if (t <= 0.0) return 0.0f;
    else if (t >= 1.0) return 1.0f;
    return pow(2.0, -10.0 * t) * sin((t * 10.0 - 0.75) * (2.0 * GLM_PI) / 3.0) + 1.0;
}

#endif
