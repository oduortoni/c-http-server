#include "header.h"

// clang-format off
static char const html_template[] =
"<html><body style='height:100vh;display:flex;justify-content:center;align-items:center;'>"
"<h1>404 Not Found</h1>"
"<p>The requested path '%.*s' was not found on this server.</p>"
"</body></html>";
// clang-format on

int
Error404(ResponseWriter* w, Request* r)
{
        // Set status code and text
        SetStatus(w, 404, "Not Found");

        // Set headers
        SetHeader(w, "Content-Type", "text/html");

        // Construct response body
        char body[1024];
        snprintf(body, sizeof(body) - 1, html_template, (int)r->path.size,
                 r->path.data);

        // Write response body
        w->WriteString(w, body);

        return EXIT_SUCCESS;
}
