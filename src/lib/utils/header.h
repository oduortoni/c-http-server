#ifndef _UTILS_HEADER_H
#define _UTILS_HEADER_H

#include <stddef.h>
#include <string.h>

#define ARRAY_LEN(a) (sizeof(a) / sizeof(*(a)))

struct String {
        char* ptr;
        size_t size;
};

struct String read_entire_file(char const* filename);

static inline bool
string_ends_with(char const* str, char const* suffix)
{
        size_t str_len    = strlen(str);
        size_t suffix_len = strlen(suffix);
        return suffix_len < str_len &&
               memcmp(str + str_len - suffix_len, suffix, suffix_len) == 0;
}

#endif  // _UTILS_HEADER_H