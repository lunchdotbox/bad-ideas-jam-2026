#ifndef ENGINE_UTILITIES_SWAP_H
#define ENGINE_UTILITIES_SWAP_H

#define SWAP(a, b)\
    {__typeof__((a)) _temp = (a);\
    (a) = (b);\
    (b) = (_temp);}

#endif
