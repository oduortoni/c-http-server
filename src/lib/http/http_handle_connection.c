#include "header.h"

int
http_handle_connection(RequestContext* context, Client client)
{
        Router* router = (Router*)context->router;
        puts("http/handle_connection.c");
        for (size_t i = 0;
             i < ARRAY_LEN(router->patterns) && router->patterns[i]; i++) {
                printf("HCRT: %s\n", router->patterns[i]);
        }
        printf("PROCESSOR COMPONENTS: %s\n", router->patterns[1]);
        http_handle(router, client);
        return 0;
}
