#include "header.h"

#include <assert.h>
#include <sys/stat.h>

static inline bool string_ends_with(char const *str, char const *suffix)
{
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    return suffix_len < str_len && memcmp(str + str_len - suffix_len, suffix, suffix_len) == 0;
}

static char const *get_mime_type(char const *filename)
{
    static struct
    {
        char const *extension;
        char const *mime_type;
    } table[] = {
        {".html", "text/html"},
        {".css", "text/css"},
        {".js", "text/javascript"},
    };
    for (size_t i = 0; i < ARRAY_LEN(table); ++i)
    {
        if (string_ends_with(filename, table[i].extension))
        {
            return table[i].mime_type;
        }
    }
    return "application/octet-stream";
}

static void close_file(void *file)
{
    fclose(*(FILE **)file);
}

struct String
{
    char *ptr;
    size_t size;
};

static struct String read_entire_file(char const *filename)
{
    __attribute__((cleanup(close_file)))
    FILE *file = fopen(filename, "r");
    struct stat st;
    if (!file || stat(filename, &st) == -1 || st.st_size == 0)
    {
        return (struct String){0};
    }
    char *buf = malloc(st.st_size);
    if (fread(buf, 1, st.st_size, file) <= 0)
    {
        return (struct String){.ptr = buf};
    }
    return (struct String){buf, st.st_size};
}

int Static(ResponseWriter *w, Request *r)
{
    // skip initial '/' to make path relative to the current working directory
    assert(strlen(r->path));
    char const *filename = r->path + 1;
    printf("Requested static file: '%s'\n", filename);

    struct stat st;
    if (r->path_regex->re_nsub != 1 || stat(filename, &st) < 0)
    {
        perror("Requested file not found");
        return Error404(w, r);
    }

    SetStatus(w, 200, "OK");
    SetHeader(w, "Content-Type", get_mime_type(filename));

    struct String html = read_entire_file(filename);
    w->Write(w, html.ptr, html.size);
    free(html.ptr);

    return 0;
}
