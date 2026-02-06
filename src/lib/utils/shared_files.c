#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "header.h"

static void
close_file(void* file)
{
        fclose(*(FILE**)file);
}

struct String
read_entire_file(char const* filename)
{
        __attribute__((cleanup(close_file))) FILE* file = fopen(filename, "r");
        struct stat st;
        if (!file || stat(filename, &st) == -1 || st.st_size == 0) {
                return (struct String){0};
        }
        char* buf = malloc(st.st_size + 1);
        if (!buf) {
                return (struct String){0};
        }
        size_t n = fread(buf, 1, st.st_size, file);
        if (n <= 0) {
                free(buf);
                return (struct String){0};
        }
        buf[n] = '\0';
        return (struct String){buf, n};
}
