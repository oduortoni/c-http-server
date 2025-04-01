#include "header.h"

int About(ResponseWriter* w, Request* r) {
    // Set status and headers
    SetStatus(w, 200, "OK");
    SetHeader(w, "Content-Type", "text/html");
    
    // Construct the About page
    char about_page[] =
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "    <title>About Page</title>"
        "    <style>"
        "        body { font-family: Arial, sans-serif; max-width: 600px; margin: 0 auto; padding: 20px; }"
        "    </style>"
        "</head>"
        "<body>"
        "    <h1>About This Server</h1>"
        "    <p>This is a simple HTTP server written in C.</p>"
        "    <p>It supports basic HTTP requests and serves static content.</p>"
        "</body>"
        "</html>";
    
    // Write the About page to the response
    w->WriteString(w, about_page);
    
    return 0;
}
