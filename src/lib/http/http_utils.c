#include <stddef.h>

#include "utils/macros.h"
#include "utils/string/header.h"

char const*
get_mime_type(char const* filename)
{
        static struct {
                char const* extension;
                char const* mime_type;
        } table[] = {
            {".html", "text/html"},
            {".css", "text/css"},
            {".js", "text/javascript"},
        };
        for (size_t i = 0; i < ARRAY_LEN(table); ++i) {
                if (string_ends_with(filename, table[i].extension)) {
                        return table[i].mime_type;
                }
        }
        return "application/octet-stream";
}
