#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"

struct BinaryString
sb_finalize(struct StringBuilder* sb)
{
        if (sb == nullptr) return (struct BinaryString){0};
        struct BinaryString result = sb->binary;
        bzero(sb, sizeof *sb);
        return result;
}

// Internal helper to ensure at least 'needed' bytes are available.
// Does not modify `sb` in case of en error.
static inline bool
sb__ensure_capacity(struct StringBuilder* sb, size_t needed)
{
        if (sb->capacity >= needed) return true;

        size_t new_cap = sb->capacity == 0 ? BUFSIZ : sb->capacity;
        while (new_cap < needed) {
                new_cap *= 2;
        }

        unsigned char* new_data = realloc(sb->binary.data, new_cap);
        if (new_data == nullptr) return false;

        sb->binary.data = new_data;
        sb->capacity    = new_cap;
        return true;
}

static inline bool
sb__correct(struct StringBuilder* sb)
{
        if (!sb || sb->binary.size > sb->capacity) return false;
        if (sb->binary.data == nullptr && (sb->binary.size || sb->capacity))
                return false;
        return true;
}

#define buffer sb->ascii.data + sb->ascii.size

bool
sb_appendf(struct StringBuilder* sb, char const* fmt, ...)
{
        if (!sb__correct(sb)) return false;

        va_list args;
        size_t available = sb->capacity - sb->ascii.size;

        // n < 0 on error
        // n == strlen of the data to be written
        // n >= available on overflow
        va_start(args, fmt);
        int n = vsnprintf(buffer, available, fmt, args);
        va_end(args);

        if (n < 0 || !sb__ensure_capacity(sb, sb->ascii.size + n)) {
                goto fail;
        }

        // Check for possible overflow
        // SAFETY: previous condition prevents negative values
        if ((size_t)n >= available) {
                va_start(args, fmt);
                n = vsnprintf(buffer, n + 1, fmt, args);
                va_end(args);

                if (n < 0) goto fail;
        }

        sb->ascii.size += n;

        return true;

fail:
        free(sb->ascii.data);
        bzero(sb, sizeof *sb);
        return false;
}

bool
sb_append_binary(struct StringBuilder* sb, unsigned char const* data,
                 size_t size)
{
        if (!sb__correct(sb)) return false;
        if (!sb__ensure_capacity(sb, sb->binary.size + size)) return false;
        memcpy(buffer, data, size);
        sb->binary.size += size;
        sb->contains_binary_data = true;
        return true;
}
