#include "header.h"

RouterStatus
handleFunc(const char* pattern, HandlerFunc handler)
{
        if (!pattern || !handler) {
                return ROUTER_INVALID;
        }

        if (!http.router) {
                http.router = calloc(1, sizeof *http.router);
                if (!http.router) {
                        return ROUTER_NOMEM;
                }
                http.router->route_count = 0;
        }

        for (int i = 0; i < http.router->route_count; i++) {
                // prevent duplicate route registration
                if (strcmp(http.router->patterns[i], pattern) == 0) {
                        return ROUTER_DUPLICATE;
                }
                continue;
        }

        if (http.router->route_count >=
            MAX_ROUTES) {  // TODO: replace with dynamic ARRAY_LEN equivalent
                           // for now
                return ROUTER_FULL;
        }

        int i = http.router->route_count;
        if (regcomp(&http.router->regex_patterns[i], pattern, REG_EXTENDED)) {
                return ROUTER_INVALID_REGEX;
        }

        http.router->patterns[i] = strdup(pattern);
        if (!http.router->patterns[i]) {
                return ROUTER_NOMEM;
        }

        http.router->handlers[i] = handler;
        http.router->route_count++;
        return ROUTER_OK;
}
