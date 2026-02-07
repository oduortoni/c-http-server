#include <assert.h>
#include <sys/stat.h>

#include "header.h"
#include "utils/header.h"

int
Static(ResponseWriter* w, Request* r)
{
        // skip initial '/' to make path relative to the current working
        // directory
        assert(strlen(r->path));
        char const* filename = r->path + 1;
        printf("Requested static file: '%s'\n", filename);

        struct stat st;
        if (r->path_regex->re_nsub != 1 || stat(filename, &st) < 0) {
                perror("Requested file not found");
                return Error404(w, r);
        }

        SetStatus(w, 200, "OK");
        SetHeader(w, "Content-Type", get_mime_type(filename));

        struct String html = read_entire_file(filename);
        w->Write(w, html.data, html.size);
        free(html.data);

        return 0;
}
