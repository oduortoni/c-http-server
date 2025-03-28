#include "./lib/http/header.h"
#include "./lib/env/header.h"

const int PORT = 9000;
const char* HOST = "127.0.0.1";

int Index(ResponseWriter w, Request r);
int About(ResponseWriter w, Request r);

int main() {
    char hostname[120];

    char host_url[80];
    const char *host = get_env_variable("HOST");
    if(host) {
        sprintf(host_url, "%s", host);
    } else {
        sprintf(host_url, "%s", HOST);
    }
    const char *port = get_env_variable("PORT");
    if(port) {
        sprintf(hostname, "%s:%s", host_url, port);
    } else {
        sprintf(hostname, "%s:%d", host_url, PORT);
    }

    http.HandleFunc("/", Index);
    http.HandleFunc("/about", About);

    Router router = {{"/","/about",NULL}, {Index,About,NULL}};
    printf("Route: %s\n", router.patterns[1]);

    http.ListenAndServe(hostname, router);

    printf("Server listening on %d\n", PORT);
    return 0;
}

int Index(ResponseWriter w, Request r) {
    w.Write("Homepage");
    return 0;
}

int About(ResponseWriter w, Request r) {
    w.Write("About Page");
    return 0;
}
