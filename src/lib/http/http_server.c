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

        if (router == NULL) {
                printf("ERROR: Router is NULL\n");
                return -1;
        }

        puts("http/server.c");
        printf("Router address: %p\n", (void*)router);
        printf("Dispatcher address: %p\n", (void*)router->dispatcher);

        if (!router->dispatcher) {
                for (size_t i = 0;
                     i < ARRAY_LEN(router->patterns) && router->patterns[i];
                     i++) {
                        printf("SERVERT: %s\n", router->patterns[i]);
                }
        } else {
                printf("Using dispatcher-based router\n");
        }
        printf("RouteD\n");
        RequestContext context = {.router = http.router};

        net_serve(host, http_handle_connection, &context);
        return 0;
}
