#ifndef INCLUDE_STRING_HEADER_H_
#define INCLUDE_STRING_HEADER_H_

#include <stddef.h>
#include <stdint.h>

// Can be either a pointer to the actual data or just a string slice.
//
// You can use `STRING_FORMAT` and `STRING_PRINT` macros to output not null
// terminated strings and slices.
struct String {
        char* data;
        size_t size;
};

#define STRING_FORMAT "%.*s"

// Type checks `str` argument and causes runtime crash if size assertion fails.
#define STRING_PRINT(str)                                                      \
        _Generic((str),                                                        \
            struct String: ((str).size <= INT32_MAX ? (int)(str).size          \
                                                    : (__builtin_trap(), 0))), \
            _Generic((str), struct String: (str).data)

bool string_ends_with(char const* str, char const* suffix);

struct String read_entire_file(char const* filename);

struct String sv_trim_prefix_until(struct String self, char needle);

#endif  // INCLUDE_STRING_HEADER_H_
