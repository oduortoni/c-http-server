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

        Router* router = router_create_regex();
        router_add(router, "^/$", Index);
        router_add(router, "^/404$", Error404);
        router_add(router, "^/static/(.*)$", Static);

        /*
         * Used to check for memory leaks in allocation and deallocation of
         * memory
         */
        // router_free(router);
        // printf("Freed test router\n");

        printf("Server listening on %d\n", PORT);
        http.ListenAndServe(hostname, router);

        /*
         * TODO: This is never reached due to infinite listener that stops on
         * CTRL + C
         * - Need to add a way to handle graceful shut down
         */
        printf("\n\n\t << Graceful Shutdown >>\n\n");
        router_free(router);

        return 0;
}
