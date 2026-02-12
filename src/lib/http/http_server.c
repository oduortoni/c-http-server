#include "header.h"
#include "utils/macros.h"

HttpServer http = {
    .ListenAndServe = listenAndServe,
    .HandleFunc     = handleFunc,
};

int
listenAndServe(char* host, Router* router)
{
        if (router == NULL) {
                // if no router is provided, use global http router
                router = http.router;
        } else {
                // if pprovided, make global point to it
                http.router = router;
        }
        puts("http/server.c");
        for (size_t i = 0;
             i < ARRAY_LEN(router->patterns) && router->patterns[i]; i++) {
                printf("SERVERT: %s\n", router->patterns[i]);
        }
        // printf("RouteD %s\n", router->patterns[1]);
        RequestContext context = {.router = router};

        net_serve(host, http_handle_connection, &context);
        return 0;
}
