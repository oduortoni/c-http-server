#ifndef INCLUDE_ARENA_HEADER_H_
#define INCLUDE_ARENA_HEADER_H_

#include <stddef.h>

typedef struct Arena Arena;
struct Arena {
        unsigned char* buf;
        size_t buf_len;
        size_t prev_offset;  // This will be useful for later on
        size_t curr_offset;
};

void arena_init(Arena* a, void* backing_buffer, size_t backing_buffer_length);

// Zero allocates returned memory.
void* arena_alloc_align(Arena* a, size_t size, size_t align);

// Zero allocates returned memory.
void* arena_alloc(Arena* a, size_t size);

// Allocates an array of size `n_elements` with type of provided `array` or
// extends previous one when called subsequently with the array of the same size
// of an element.
//
// Zero allocates returned memory.
#define arena_alloc_array(arena, n_elements, array)               \
        arena_alloc_align((arena), (n_elements) * sizeof(*array), \
                          alignof(typeof(*array)))

void* arena_resize_align(Arena* a, void* old_memory, size_t old_size,
                         size_t new_size, size_t align);
void* arena_resize(Arena* a, void* old_memory, size_t old_size,
                   size_t new_size);

void arena_free_all(Arena* a);

#endif  // INCLUDE_ARENA_HEADER_H_
