#pragma once
#include <stdlib.h>
#define MALLOC_CAP_SPIRAM 1
#define MALLOC_CAP_INTERNAL 4
#define MALLOC_CAP_8BIT 2
inline void *heap_caps_malloc(size_t n,int){return malloc(n);}

inline size_t heap_caps_get_free_size(int){return 128*1024;}
inline size_t heap_caps_get_largest_free_block(int){return 70*1024;}
