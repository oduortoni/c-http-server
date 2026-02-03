#include "header.h"

int handleFunc(char *pattern, HandlerFunc handler) {
    if(!http.router) {
        http.router = (Router*)calloc(1, sizeof(*http.router));
    }
    // append route for pattern/handler to the router
    for (uint i = 0; i < ARRAY_LEN(http.router->patterns); i++) {
        if (http.router->patterns[i]) continue;
        http.router->patterns[i] = strdup(pattern);
        http.router->handlers[i] = handler;
        break;
    }
    return 0;
}
