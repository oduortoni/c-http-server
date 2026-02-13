#include "header.h"

int
Index(ResponseWriter* w, Request* r)
{
        SetStatus(w, 200, "OK");
        SetHeader(w, "Content-Type", "text/html");
        char buffer[300];
        snprintf(buffer, 300, "<html><body style=\"display:flex;align-items:center;justify-content:center;background:#000;color:#fff;\"><div style=\"width:fit-content;height:fit-content;\"><h1>A Minimalistic C Server</h1><p>Request path: %s</p></div></body></html>", r->path.data);
        w->WriteString(w, buffer);

        return EXIT_SUCCESS;
}
