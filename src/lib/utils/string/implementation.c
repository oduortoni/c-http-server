#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "./header.h"

bool
string_ends_with(char const* str, char const* suffix)
{
        size_t str_len    = strlen(str);
        size_t suffix_len = strlen(suffix);
        return suffix_len < str_len &&
               memcmp(str + str_len - suffix_len, suffix, suffix_len) == 0;
}

static void
close_file(void* file)
{
        fclose(*(FILE**)file);
}

[[nodiscard]]
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

struct String
sv_trim_prefix_until(struct String self, char needle)
{
        while (self.size && *self.data != needle) {
                self.data++, self.size--;
        }
        return self;
}
