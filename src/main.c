#include "./lib/net/header.h"

const int PORT = 9000;

int Index(ResponseWriter w, Request r);

int main() {
    char host[120];
    sprintf(host, "127.0.0.1:%d", PORT);

    http.HandleFunc("/", Index);

    Router router;
    http.ListenAndServe(host, router);

    printf("Server listening on %d\n", PORT);
    return 0;
}

int Index(ResponseWriter w, Request r) {
    w.Write("Hello there");
}
