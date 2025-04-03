#include "header.h"

char html[] =
"<html><body style='height:100vh;display:flex;justify-content:center;align-items:center;'>"
"<h1>404 Not Found</h1>"
"<p>The requested path '%s' was not found on this server.</p>"
"</body></html>";

int Error404(ResponseWriter* w, Request* r) {
    // Set status code and text
    SetStatus(w, 404, "Not Found");
    
    // Set headers
    SetHeader(w, "Content-Type", "text/html");
    
    // Construct response body
    char body[1024];
    snprintf(body, sizeof(body), html, r->path);
    
    // Write response body
    w->WriteString(w, body);
    
    return 0;
}
