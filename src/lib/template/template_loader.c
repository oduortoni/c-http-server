#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"

char*
template_load(const char* filename)
{
        FILE* file = fopen(filename, "rb");
        if (!file) return nullptr;

        char* buffer    = nullptr;
        size_t size     = 0;
        size_t capacity = 0;

        for (;;) {
                if (size + BUFSIZ > MAX_TEMPLATE_SIZE) {
                        errno = EFBIG;
                        goto fail;
                }

                if (size + BUFSIZ + 1 > capacity) {
                        capacity = capacity ? capacity * 2 : BUFSIZ;
                        if (capacity > MAX_TEMPLATE_SIZE + 1)
                                capacity = MAX_TEMPLATE_SIZE + 1;

                        char* tmp = realloc(buffer, capacity);
                        if (!tmp) goto fail;

                        buffer = tmp;
                }

                size_t n = fread(buffer + size, 1, BUFSIZ, file);
                size += n;

                if (n < BUFSIZ) {
                        if (ferror(file)) goto fail;
                        break;
                }
        }

        buffer[size] = '\0';
        fclose(file);
        return buffer;

fail:
        /**
         * Cleanup on failure
         */
        free(buffer);
        fclose(file);
        return nullptr;
}

void
template_free(char* template)
{
        free(template);
}
