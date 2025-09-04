#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>

void init_memory(void);
void* malloc(size_t size);
void free(void* ptr);

#endif
