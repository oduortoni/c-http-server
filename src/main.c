#include "app/header.h"
#include "lib/env/header.h"
#include "lib/http/header.h"

const int PORT   = 9000;
const char* HOST = "127.0.0.1";

int
main()
{
        char hostname[120];

        char host_url[80];
        const char* host = env_get_variable("HOST");
        if (host) {
                sprintf(host_url, "%s", host);
        } else {
                sprintf(host_url, "%s", HOST);
        }
        const char* port = env_get_variable("PORT");
        if (port) {
                sprintf(hostname, "%s:%s", host_url, port);
        } else {
                sprintf(hostname, "%s:%d", host_url, PORT);
        }

        http.HandleFunc("^/$", Index);
        http.HandleFunc("^/about$", About);
        http.HandleFunc("^/404$", Error404);
        http.HandleFunc("^/static/(.*)$", Static);

        http.ListenAndServe(hostname, NULL);

        // Router router = {{"/404", "/", "/about", NULL}, {Error404, Index,
        // About, NULL}}; printf("Route: %s\n", router.patterns[1]);
        // http.ListenAndServe(hostname, &router);

        printf("Server listening on %d\n", PORT);
        return 0;
}
