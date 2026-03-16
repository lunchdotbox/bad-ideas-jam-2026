#ifndef ENGINE_UTILITIES_COMPARISON_H
#define ENGINE_UTILITIES_COMPARISON_H

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(v, min, max) (MAX(min, MIN(max, v)))

#endif
