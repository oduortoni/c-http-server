#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/header.h"

bool
print_escaped(char const* start, char const* end, struct StringBuilder* sb)
{
#define output(...)                                    \
        do {                                           \
                bool ok = sb_appendf(sb, __VA_ARGS__); \
                if (!ok) return false;                 \
        } while (0)

        for (auto p = start; p < end; ++p) {
                switch (*p) {
                case '\"':
                        output("%s", "\\\"");
                        break;
                case '\\':
                        output("%s", "\\\\");
                        break;
                case '\n':
                        output("%s", "\\n");
                        break;
                case '\r':
                        output("%s", "\\r");
                        break;
                default:
                        output("%c", *p);
                }
        }

        return true;

#undef output
}

char*
trim(char* str)
{
        while (isspace(*str)) str++;
        if (*str == 0) return str;
        char* end = str + strlen(str) - 1;
        while (end > str && isspace(*end)) end--;
        end[1] = '\0';
        return str;
}

int
translate(const char* const name, char const* const src)
{
        auto len = strlen(name);
        char guard[len + 1];
        guard[len] = '\0';
        for (size_t i = 0; i < len; i++) {
                guard[i] = toupper((unsigned char)name[i]);
                if (guard[i] == '/') guard[i] = '_';
        }

        struct StringBuilder sb = {0};

#define outputf(...)                                    \
        do {                                            \
                bool ok = sb_appendf(&sb, __VA_ARGS__); \
                if (!ok) {                              \
                        free(sb.ascii.data);            \
                        return EXIT_FAILURE;            \
                }                                       \
        } while (0)
#define output(...) outputf("%s", __VA_ARGS__)

#define print_escaped(...)                            \
        do {                                          \
                bool ok = print_escaped(__VA_ARGS__); \
                if (!ok) {                            \
                        free(sb.ascii.data);          \
                        return EXIT_FAILURE;          \
                }                                     \
        } while (0)

        // Header Guard
        outputf("#ifndef %s_H\n#define %s_H\n\n", guard, guard);
        output(
            "bool render_template("
            "struct Context* ctx, struct StringBuilder* sb);\n\n");
        output("#endif\n\n#ifdef IMPLEMENTATION\n\n");
        output("#define $ (*ctx)\n");
        output(
            "#define sb_appendf(...) do { "
            "bool res = sb_appendf(__VA_ARGS__); if (!res) return false; "
            "} while(0)\n\n");
        output(
            "bool\nrender_template("
            "struct Context* ctx, struct StringBuilder* sb)\n{\n");

        char const* cursor        = src;
        int indentation_level     = 1;
        constexpr int indentation = 4;
        while (*cursor) {
                char* tag_open = strstr(cursor, "{{");

                if (!tag_open) {
                        // Remainder of file
                        outputf("%*ssb_appendf(sb, \"",
                                indentation_level * indentation, "");
                        print_escaped(cursor, cursor + strlen(cursor), &sb);
                        output("\");\n");
                        break;
                }

                // 1. Literal text before tag
                struct String text_before_tag = {0};
                if (tag_open > cursor) {
                        text_before_tag.data = (char*)cursor;
                        text_before_tag.size = tag_open - cursor;
                }

                // 2. Parse Tag
                char* tag_contents = tag_open + 2;
                char* tag_close    = strstr(tag_contents, "}}");

                if (!tag_close) {
                        int starting_line = 1;
                        for (auto it = src; it < tag_open; ++it) {
                                starting_line += *it == '\n';
                        }
                        fprintf(stderr,
                                "ERROR: Template statement has unmatched "
                                "parentecies\n%s:%d:\n",
                                name, starting_line);
                        auto tail_len = strlen(tag_open);
                        if (tail_len > 40) tail_len = 40;
                        tag_open[tail_len] = '\0';
                        fprintf(stderr, "%s", tag_open);
                        free(sb.ascii.data);
                        return EXIT_FAILURE;
                }

                bool line_elimination =
                    (tag_close > tag_contents && *(tag_close - 1) == '-');

                // Extract inner content
                size_t content_len = tag_close - tag_contents;
                if (line_elimination) content_len--;

                char* raw_content = strndup(tag_contents, content_len);
                char* code        = trim(raw_content);

                cursor            = tag_close + 2;

                // 3. Handle Line Elimination
                if (line_elimination) {
                        // Skip trailing white-spaces and exactly one newline
                        while (isspace(*cursor) && *cursor != '\n') {
                                cursor++;
                        }
                        if (*cursor == '\n') ++cursor;

                        // Skip prepended white-spaces
                        while (text_before_tag.size) {
                                char last = text_before_tag
                                                .data[text_before_tag.size - 1];
                                if (isspace(last) && last != '\n') {
                                        text_before_tag.size--;
                                } else {
                                        break;
                                }
                        }
                }

                // Output text before tag
                if (text_before_tag.size) {
                        outputf("%*ssb_appendf(sb, \"",
                                indentation_level * indentation, "");
                        print_escaped(
                            text_before_tag.data,
                            text_before_tag.data + text_before_tag.size, &sb);
                        output("\");\n");
                }

                // Output tag body
                size_t const code_len = strlen(code);
                if (strncmp(code, "$.", 2) == 0) {
                        // String Mode shortcut
                        outputf("%*ssb_appendf(sb, \"%%s\", %s);\n",
                                indentation_level * indentation, "", code);
                } else if (code[0] == '\"') {
                        // Format Mode shortcut
                        outputf("%*ssb_appendf(sb, %s);\n",
                                indentation_level * indentation, "", code);
                } else if (code_len) {
                        // Raw Mode (C code)
                        // count brackets balance for indentation
                        int brackets = 0;
                        for (size_t i = 0; i < code_len; ++i) {
                                brackets += code[i] == '{';
                                brackets -= code[i] == '}';
                        }
                        // decrease indentation before tag's body
                        if (brackets < 0) indentation_level += brackets;
                        outputf("%*s%s\n", indentation_level * indentation, "",
                                code);
                        // increase indentation after tag's body
                        if (brackets > 0) indentation_level += brackets;
                }
                free(raw_content);
        }

        output("    return true;\n");
        output("}\n\n#undef sb_appendf\n#undef $\n#endif\n");

        printf("%s", sb.ascii.data);
        free(sb.ascii.data);

        return EXIT_SUCCESS;
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

        auto exit_code = translate(filename, content.data);

        free(filename);
        free(content.data);

        return exit_code;
}
