#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/header.h"

void
print_escaped(const char* start, const char* end)
{
        for (auto p = start; p < end; ++p) {
                switch (*p) {
                case '\"':
                        printf("\\\"");
                        break;
                case '\\':
                        printf("\\\\");
                        break;
                case '\n':
                        printf("\\\n");
                        break;
                case '\r':
                        printf("\\\r");
                        break;
                default:
                        putchar(*p);
                        break;
                }
        }
}

char*
trim(char* str)
{
        while (isspace((unsigned char)*str)) str++;
        if (*str == 0) return str;
        char* end = str + strlen(str) - 1;
        while (end > str && isspace((unsigned char)*end)) end--;
        end[1] = '\0';
        return str;
}

void
translate(const char* name, char* src)
{
        char guard[256];
        auto len = strlen(name);
        for (size_t i = 0; i < len; i++) {
                guard[i] = (char)toupper((unsigned char)name[i]);
                if (guard[i] == '/') guard[i] = '_';
        }
        guard[len] = '\0';

        // Header Guard
        printf("#ifndef %s_H\n#define %s_H\n\n", guard, guard);
        printf(
            "bool render_template(struct Context* ctx, struct StringBuilder* "
            "sb);\n\n");
        printf("#endif\n\n#ifdef IMPLEMENTATION\n\n");
        printf("#define $ (*ctx)\n");
        printf(
            "#define sb_appendf(...) do { bool res = sb_appendf(__VA_ARGS__); "
            "if (!res) return false; } while(0)\n\n");
        printf(
            "bool\nrender_template(struct Context* ctx, struct StringBuilder* "
            "sb)\n{\n");

        char* cursor = src;
        while (*cursor) {
                char* tag_open = strstr(cursor, "{{");

                if (!tag_open) {
                        // Remainder of file
                        printf("    sb_appendf(sb, \"");
                        print_escaped(cursor, cursor + strlen(cursor));
                        printf("\");\n");
                        break;
                }

                // 1. Literal text before tag
                if (tag_open > cursor) {
                        printf("    sb_appendf(sb, \"");
                        print_escaped(cursor, tag_open);
                        printf("\");\n");
                }

                // 2. Parse Tag
                char* tag_contents = tag_open + 2;
                char* tag_close    = strstr(tag_contents, "}}");
                if (!tag_close) break;

                bool line_elimination =
                    (tag_close > tag_contents && *(tag_close - 1) == '-');

                // Extract inner content
                size_t content_len = (size_t)(tag_close - tag_contents);
                if (line_elimination) content_len--;

                char* raw_content = strndup(tag_contents, content_len);
                char* code        = trim(raw_content);

                // Logic Modes
                if (strncmp(code, "$.", 2) == 0) {
                        // String Mode shortcut
                        printf("    sb_appendf(sb, \"%%s\", %s);\n", code);
                } else if (code[0] == '\"') {
                        // Format Mode shortcut
                        printf("    sb_appendf(sb, %s);\n", code);
                } else {
                        // Raw Mode (C code)
                        printf("    %s\n", code);
                }

                free(raw_content);

                // 3. Handle Line Elimination
                cursor = tag_close + 2;
                if (line_elimination) {
                        // Skip trailing whitespace and exactly one newline
                        while (*cursor == ' ' || *cursor == '\t') cursor++;
                        if (*cursor == '\r') cursor++;
                        if (*cursor == '\n') cursor++;
                }
        }

        printf("    return true;\n");
        printf("}\n\n#undef sb_appendf\n#undef $\n#endif\n");
}

int
main(int argc, char** argv)
{
        if (argc < 2) return fprintf(stderr, "Usage: thc <file.th>\n"), 1;

        auto content = read_entire_file(argv[1]);
        if (!content.data) return perror("file"), 1;

        // Get filename without extension for the guard
        char* filename = strdup(argv[1]);
        char* ext      = strrchr(filename, '.');
        if (ext) *ext = '\0';

        translate(filename, content.data);

        free(filename);
        free(content.data);
        return 0;
}
