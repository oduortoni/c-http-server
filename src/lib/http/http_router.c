#include "header.h"
#include "utils/macros.h"

HandlerFunc
router_match(Router* router, const char* path, Request* req)
{
        for (int i = 0; i < router->route_count; i++) {
                req->path_regex = &router->regex_patterns[i];
                if (!regexec(req->path_regex, path,
                             ARRAY_LEN(req->path_matches), req->path_matches,
                             0)) {
                        return router->handlers[i];
                }
        }
        return nullptr;
}
