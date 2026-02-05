#include "header.h"

static char const response_template[] =
    "<div class='response success'>"
    "<h3>Thank you for your submission!</h3>"
    "<p><strong>Name:</strong> %s</p>"
    "<p><strong>Email:</strong> %s</p>"
    "<p><strong>Message:</strong> %s</p>"
    "</div>";

int Index(ResponseWriter *w, Request *r)
{
    char *html_template = template_load("src/app/templates/index.html");
    if (!html_template) {
        SetStatus(w, 500, "Internal Server Error");
        w->WriteString(w, "Template not found");
        return -1;
    }

    FormData form_data = {0};
    char response_message[512] = "";

    if (strcmp(r->method, "POST") == 0)
    {
        printf("Form submitted with data:\n%s\n", r->body);
        parse_form_data(r->body, &form_data);

        const char *name = get_form_value(&form_data, "name");
        const char *email = get_form_value(&form_data, "email");
        const char *message = get_form_value(&form_data, "message");

        snprintf(response_message, sizeof(response_message) - 1, response_template, name, email, message);
    }

    char response[8192];
    snprintf(response, sizeof(response) - 1, html_template, response_message);

    SetStatus(w, 200, "OK");
    SetHeader(w, "Content-Type", "text/html");
    w->WriteString(w, response);

    // free template
    template_free(html_template);

    return 0;
}
