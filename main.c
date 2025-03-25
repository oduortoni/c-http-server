#include <stdio.h>
#include "header.h"

const int PORT = 9000;

HandlerFunc Index(ResponseWriter w, Request r);

int main() {
    char host[120];
    sprintf(host, "localhost:%d", PORT);

    Server server;
    server.HandleFunc("/", Index);

    Router router;
    server.ListenAndServe(host, router);

    printf("Server listening on %d\n", PORT);
    return 0;
}

HandlerFunc Index(ResponseWriter w, Request r) {
    w.Write("Hello there");
}
