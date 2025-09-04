#ifndef STRING_H
#define STRING_H

#include <stddef.h>

static inline size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

static inline void* memcpy(void* dest, const void* src, size_t n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

static inline void* memset(void* s, int c, size_t n) {
    unsigned char* p = (unsigned char*)s;
    for (size_t i = 0; i < n; i++) {
        p[i] = (unsigned char)c;
    }
    return s;
}

#endif
