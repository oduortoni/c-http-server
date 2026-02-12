#include "header.h"

// clang-format off
static char const html[] =
"<!DOCTYPE html>"
"<html>"
"<head>"
"    <title>About Page</title>"
"    <style>"
"        body { font-family: Arial, sans-serif; max-width: 600px; margin: 0 auto; padding: 20px; background: #000000; color: #ffffff;}"
"    </style>"
"</head>"
"<body>"
"    <h1>Our Blog</h1>"
"    <p>This is the blog for this site.</p>"
"</body>"
"</html>";
// clang-format on

int
BlogIndex(ResponseWriter* w, Request* r)
{
        (void)r;
        // Set status and headers
        SetStatus(w, 200, "OK");
        SetHeader(w, "Content-Type", "text/html");

        // Write the About page to the response
        w->WriteString(w, html);

        return EXIT_SUCCESS;
}
