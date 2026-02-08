#include "header.h"

void
url_decode(char* dest, const char* src)
{
        int j = 0;
        for (int i = 0; src[i] && j < MAX_FIELD_LENGTH - 1; i++) {
                if (src[i] == '+') {
                        dest[j++] = ' ';
                } else if (src[i] == '%' && isxdigit(src[i + 1]) &&
                           isxdigit(src[i + 2])) {
                        char hex[3] = {src[i + 1], src[i + 2], '\0'};
                        dest[j++]   = (char)strtol(hex, NULL, 16);
                        i += 2;
                } else {
                        dest[j++] = src[i];
                }
        }
        dest[j] = '\0';
}

void
parse_form_data(const char* body, FormData* form_data)
{
        form_data->count = 0;
        char* saveptr;
        char* token = strtok_r((char*)body, "&", &saveptr);

        while (token != NULL && form_data->count < MAX_FORM_FIELDS) {
                char* eq = strchr(token, '=');
                if (eq) {
                        *eq         = '\0';
                        char* key   = token;
                        char* value = eq + 1;

                        url_decode(form_data->fields[form_data->count].value,
                                   value);
                        strncpy(form_data->fields[form_data->count].name, key,
                                MAX_FIELD_LENGTH - 1);
                        form_data->fields[form_data->count]
                            .name[MAX_FIELD_LENGTH - 1] = '\0';

                        form_data->count++;
                }
                token = strtok_r(NULL, "&", &saveptr);
        }
}

const char*
get_form_value(const FormData* form_data, const char* name)
{
        for (int i = 0; i < form_data->count; i++) {
                if (strcmp(form_data->fields[i].name, name) == 0) {
                        return form_data->fields[i].value;
                }
        }
        return nullptr;
}
