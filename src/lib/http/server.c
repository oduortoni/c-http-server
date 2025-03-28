#include "header.h"

HttpServer http = {listenAndServe, handleFunc};

int listenAndServe(char *host, Router router) {
  printf("RouteD %s\n", router.patterns[1]);
    Processor processor = {handle_connection, &router};
    serve(host, processor);
    return 0;
}
