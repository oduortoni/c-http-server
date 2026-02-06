#include "header.h"
#include "utils/header.h"

// clang-format off
static char const response_template[] =
    "<div class='response success'>"
    "<h3>Thank you for your submission!</h3>"
    "<p><strong>Name:</strong> %s</p>"
    "<p><strong>Email:</strong> %s</p>"
    "<p><strong>Message:</strong> %s</p>"
    "</div>";
// clang-format on

int
Index(ResponseWriter* w, Request* r)
{
        const char* index_path      = TEMPLATE_PATH "index.html";
        struct String html_template = read_entire_file(index_path);
        if (!html_template.ptr) {
                /* nothing to clean up */
                SetStatus(w, 500, "Internal Server Error");
                w->WriteString(w, "Template not found");
                return -1;
        }

        FormData form_data         = {0};
        char response_message[512] = "";

        if (strcmp(r->method, "POST") == 0) {
                parse_form_data(r->body, &form_data);

                const char* name    = get_form_value(&form_data, "name");
                const char* email   = get_form_value(&form_data, "email");
                const char* message = get_form_value(&form_data, "message");

                if (!name || !email || !message) {
                        SetStatus(w, 400, "Missing form fields");
                        free(html_template.ptr);
                        return -1;
                }

                int n = snprintf(response_message, sizeof(response_message),
                                 response_template, name, email, message);

                if (n < 0 || n >= (int)sizeof(response_message)) {
                        SetStatus(w, 500, "Response too large");
                        free(html_template.ptr);
                        return -1;
                }
        }

        char response[8192];
        int n = snprintf(response, sizeof(response), html_template.ptr,
                         response_message);
        free(html_template.ptr);

        if (n < 0 || n >= (int)sizeof(response)) {
                SetStatus(w, 500, "Rendered page too large");
                return -1;
        }

        SetStatus(w, 200, "OK");
        SetHeader(w, "Content-Type", "text/html");
        w->WriteString(w, response);

        return 0;
}
