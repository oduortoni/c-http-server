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
        }

        for (size_t i = 0; i < ARRAY_LEN(http.router->patterns); i++) {
                if (http.router->patterns[i]) {
                        // prevent duplicate route registration
                        if (strcmp(http.router->patterns[i], pattern) == 0) {
                                return ROUTER_DUPLICATE;
                        }
                        continue;
                }

                if (regcomp(&http.router->regex_patterns[i], pattern,
                            REG_EXTENDED)) {
                        return ROUTER_INVALID_REGEX;
                }

                http.router->patterns[i] = strdup(pattern);
                if (!http.router->patterns[i]) {
                        return ROUTER_NOMEM;
                }

                http.router->handlers[i] = handler;
                return ROUTER_OK;
        }

        return ROUTER_FULL;
}
