#include <stdio.h>
#include "header.h"

const int PORT = 9000;

int Index(ResponseWriter w, Request r);

HttpServer http = {httpListener, handleFunc};

int main() {
    char host[120];
    sprintf(host, "localhost:%d", PORT);

    http.HandleFunc("/", Index);

    Router router;
    http.ListenAndServe(host, router);

    printf("Server listening on %d\n", PORT);
    return 0;
}

int Index(ResponseWriter w, Request r) {
    w.Write("Hello there");
}
