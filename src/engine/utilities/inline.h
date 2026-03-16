#ifndef ENGINE_UTILITIES_INLINE_H
#define ENGINE_UTILITIES_INLINE_H

#if defined(_MSC_VER)
#define INLINE __forceinline
#else
#define INLINE static inline __attribute((always_inline))
#endif

#endif
