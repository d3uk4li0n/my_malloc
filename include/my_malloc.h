#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>

#ifndef UTILITY_C
#define UTILITY_C

#define CHUNK_SIZE (1 << 16)

size_t chunk_align(size_t);

#endif

#ifndef MALLOC_C
#define MALLOC_C

//template of my_malloc function(s)
void* my_malloc(size_t size);

#endif

#ifndef REALLOC_C
#define REALLOC_C

//template of my_realloc function(s)
void* my_realloc(void* ptr, size_t size);

#endif

#ifndef CALLOC_C
#define CALLOC_C

//template of my_calloc function(s)
void* my_calloc(size_t nmemb, size_t size);

#endif

#ifndef FREE_C
#define FREE_C

//template of my_free function(s)
void my_free(void* ptr);

#endif
