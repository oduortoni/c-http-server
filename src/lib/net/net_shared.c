#include "header.h"

void
str_split(const char* input, char delimiter, char* head, char* tail)
{
        const char* delimiter_pos = strchr(input, delimiter);
        if (delimiter_pos == NULL) {
                fprintf(stderr,
                        "Error: Delimiter not found in the input string.\n");
                exit(1);
        }
        size_t head_len = delimiter_pos - input;

        strncpy(head, input, head_len);
        head[head_len] = '\0';

        strcpy(tail, delimiter_pos + 1);
}
