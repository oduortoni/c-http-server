#ifndef _STRING_BUILDER_H_
#define _STRING_BUILDER_H_

#include <stddef.h>

struct BinaryString {
        unsigned char* data;
        size_t size;
};

struct String {
        char* data;
        size_t size;
};

// Main purpose is to construct arbitrary sequences of printable ASCII
// characters or binary data.
//
// Supports zero initialization, e.g.:
//
//      struct StringBuilder sb = {0};  // correct
//
struct StringBuilder {
        // Using union to prevent type punning everywhere
        union {
                struct BinaryString binary;
                struct String ascii;
        };
        size_t capacity;
        bool contains_binary_data;
};

// Transfers ownership to underlying data.
// The caller is responsible for free().
// The struct is reset to zero for reuse.
[[nodiscard]]
struct BinaryString sb_finalize(struct StringBuilder* sb);

// Append printable ASCII data
[[gnu::format(printf, 2, 3)]]
bool sb_appendf(struct StringBuilder* sb, char const* fmt, ...);

// Append binary data
bool sb_append_binary(struct StringBuilder* sb, unsigned char const* data,
                      size_t size);

#endif  // _STRING_BUILDER_H_
