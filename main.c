#include <stdio.h>
#include "header.h"

const PORT = 9000;

HandlerFunc index(ResponseWriter w, Request r);

int main() {
    char host[120];
    sprintf(host, "localhost:%d", PORT);

    Server server;
    server.HandleFunc("/", index);

    Router router;
    server.ListenAndServe(host, router);

    printf("Server listening on %d\n", PORT);
    return 0;
}

HandlerFunc index(ResponseWriter w, Request r) {
    //
}
