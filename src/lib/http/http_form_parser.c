#include <ctype.h>

#include "header.h"
#include "utils/logging/header.h"

void
url_decode(char* dest, struct String const src)
{
        size_t j = 0;
        for (size_t i = 0; i < src.size && j < MAX_FIELD_LENGTH - 1; i++) {
                if (src.data[i] == '+') {
                        dest[j++] = ' ';
                }

                else if (i + 2 < src.size && src.data[i] == '%' &&
                         isxdigit(src.data[i + 1]) &&
                         isxdigit(src.data[i + 2])) {
                        char hex[3] = {src.data[i + 1], src.data[i + 2], '\0'};
                        dest[j++]   = strtol(hex, NULL, 16);
                        i += 2;

                } else {
                        dest[j++] = src.data[i];
                }
        }
        dest[j] = '\0';
}

void
form_data_append(FormData* form_data, struct String name, struct String value)
{
        if (form_data->count + 1 >= MAX_FORM_FIELDS) {
                // TODO: handle this case properly
                error("reached max field count");
                return;
        }

        auto field = &form_data->fields[form_data->count];

        size_t len = name.size;
        if (len > MAX_FIELD_LENGTH - 2) len = MAX_FIELD_LENGTH - 2;
        strncpy(field->name, name.data, len);
        field->name[MAX_FIELD_LENGTH - 1] = '\0';

        url_decode(field->value, value);

        form_data->count++;
}

void
parse_form_data(struct String body, FormData* form_data)
{
        debug("Form data: %.*s", STRING_PRINT(body));
        form_data->count = 0;
        auto amp         = body;
        while (amp.size) {
                auto eq   = sv_trim_prefix_until(amp, '=');
                auto name = (struct String){
                    .data = amp.data,
                    .size = eq.data - amp.data,
                };

                if (eq.size) eq.data++, eq.size--;

                amp        = sv_trim_prefix_until(eq, '&');
                auto value = (struct String){
                    .data = eq.data,
                    .size = amp.data - eq.data,
                };

                if (amp.size) amp.data++, amp.size--;

                debug("Form name: '%.*s', value: '%.*s'", STRING_PRINT(name),
                      STRING_PRINT(value));

                form_data_append(form_data, name, value);
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
