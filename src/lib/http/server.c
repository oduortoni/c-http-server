#include "header.h"

HttpServer http = {listenAndServe, handleFunc};
Router router = {0};

int listenAndServe(char *host, Router rtr) {
    router = rtr;
    Processor processor = {handle_connection};
    serve(host, processor);
    return 0;
}
