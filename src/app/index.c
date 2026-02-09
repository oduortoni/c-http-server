#include "header.h"
#include "utils/logging/header.h"
#include "utils/string-builder/header.h"

struct Context {
        char const* name;
        char const* email;
        char const* message;
        bool is_method_post;
};

#define IMPLEMENTATION
#include "templates/index.h"

int
Index(ResponseWriter* w, Request* r)
{
        struct Context ctx = {0};
        FormData form_data = {0};

        if (strcmp(r->method, "POST") == 0) {
                parse_form_data(r->body, &form_data);

                ctx.name           = get_form_value(&form_data, "name");
                ctx.email          = get_form_value(&form_data, "email");
                ctx.message        = get_form_value(&form_data, "message");
                ctx.is_method_post = true;

                debug("name: %s\nemail: %s\nmessage: %s", ctx.name, ctx.email,
                      ctx.message);

                if (!ctx.name || !ctx.email || !ctx.message) {
                        SetStatus(w, 400, "Missing form fields");
                        return -1;
                }
        }

        struct StringBuilder sb = {0};
        bool ok                 = render_template(&ctx, &sb);
        if (!ok) {
                SetStatus(w, 500, "Failed to render the page");
                return -1;
        }

        SetStatus(w, 200, "OK");
        SetHeader(w, "Content-Type", "text/html");
        w->WriteString(w, sb.ascii.data);
        free(sb.ascii.data);

        return EXIT_SUCCESS;
}
