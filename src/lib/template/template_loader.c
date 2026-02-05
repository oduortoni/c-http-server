#include "header.h"

#define READ_CHUNK 4096

char*
template_load(const char* filename)
{
        FILE* file = fopen(filename, "rb");
        if (!file) return NULL;

        char* buffer    = NULL;
        size_t size     = 0;
        size_t capacity = 0;

        for (;;) {
                if (size + READ_CHUNK > MAX_TEMPLATE_SIZE) {
                        errno = EFBIG;
                        goto fail;
                }

                if (size + READ_CHUNK + 1 > capacity) {
                        size_t new_capacity =
                            capacity ? capacity * 2 : READ_CHUNK;
                        if (new_capacity > MAX_TEMPLATE_SIZE + 1)
                                new_capacity = MAX_TEMPLATE_SIZE + 1;

                        char* tmp = realloc(buffer, new_capacity);
                        if (!tmp) goto fail;

                        buffer   = tmp;
                        capacity = new_capacity;
                }

                size_t n = fread(buffer + size, 1, READ_CHUNK, file);
                size += n;

                if (n < READ_CHUNK) {
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
        return NULL;
}

void
template_free(char* template)
{
        free(template);
}
