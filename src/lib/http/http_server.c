#include "header.h"
#include "utils/macros.h"
#include "utils/logging/header.h"

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

        if (router == NULL) {
                error("ERROR: Router is NULL\n");
                return -1;
        }

        // info("http/server.c");
        // info("Router address: %p\n", (void*)router);
        // info("Dispatcher address: %p\n", (void*)router->dispatcher);

        if (!router->dispatcher) {
                for (size_t i = 0;
                     i < ARRAY_LEN(router->patterns) && router->patterns[i];
                     i++) {
                        info("Using old router: %s\n", router->patterns[i]);
                }
        } else {
                info("Using dispatcher-based router\n");
        }
        RequestContext context = {.router = http.router};

        net_serve(host, http_handle_connection, &context);
        return 0;
}
