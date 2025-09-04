#include "string.h"

int strcmp(const char* s1, const char* s2) {
    // check each char
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    // get diff
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strlen(const char* s) {
    // count chars
    int len = 0;
    while (s[len]) {
        len++;
    }
    return len;
}
