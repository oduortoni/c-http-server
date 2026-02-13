#ifndef INCLUDE_UTILS_MACROS_H_
#define INCLUDE_UTILS_MACROS_H_

#include <stddef.h>

#define ARRAY_LEN(a) (sizeof(a) / sizeof(*(a)))

// Dynamic array is any structure that has following fields:
//
//      struct RequestHeaders {
//              // type doesn't matter
//              Header* items;
//              // type have to match
//              size_t len;
//              size_t capacity;
//      } headers;
//

#define is_same_type(value, T) _Generic((value), T: true, default: false)

#define da_append(array, value)                                                \
        do {                                                                   \
                static_assert(is_same_type((array).len, size_t));              \
                static_assert(is_same_type((array).capacity, size_t));         \
                static_assert(is_same_type(*(array).items, typeof(value)));    \
                if ((array).items == nullptr) {                                \
                        (array).len      = 0;                                  \
                        (array).capacity = 10;                                 \
                        (array).items =                                        \
                            malloc(sizeof(*(array).items) * (array).capacity); \
                }                                                              \
                if ((array).len + 1 > (array).capacity) {                      \
                        (array).capacity *= 2;                                 \
                        (array).items = realloc(                               \
                            (array).items,                                     \
                            (array).capacity * sizeof(*(array).items));        \
                }                                                              \
                if ((array).items) (array).items[(array).len++] = value;       \
        } while (0)

#endif  // INCLUDE_UTILS_MACROS_H_
