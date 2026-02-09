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

void* arena_alloc_align(Arena* a, size_t size, size_t align);
void* arena_alloc(Arena* a, size_t size);

void* arena_resize_align(Arena* a, void* old_memory, size_t old_size,
                         size_t new_size, size_t align);
void* arena_resize(Arena* a, void* old_memory, size_t old_size,
                   size_t new_size);

void arena_free_all(Arena* a);

//
// Extension to the original arena
//

// TODO: implement arena extension

// It is common for processing stream that you don't know the size of incoming
// data. This function invalidates any incomming `arena_alloc*` calls until
// `arena_vla_end`.
//
// @returns Available space in the arena, in bytes
size_t arena_vla_start(Arena* a);

// Allocates next element of VLA
//
// @param `size`: size of underlying data
void* arena_alloc_vla(Arena* a, size_t size);

// Returns the length of VLA
//
// @param `size`: size of underlying data
size_t arena_vla_end(Arena* a, size_t size);

#endif  // INCLUDE_ARENA_HEADER_H_
