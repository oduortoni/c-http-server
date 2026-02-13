#include "header.h"
#include "utils/macros.h"

HandlerFunc
router_match(Router* router, const char* path, Request* req)
{
        if (router->dispatcher) {
                return router->dispatcher->match(router->impl_data, path, req);
        }

        // leave the old logic as a fallback
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

void
router_add(Router* router, const char* pattern, HandlerFunc handler)
{
        if (router->dispatcher) {
                router->dispatcher->add_route(router->impl_data, pattern,
                                              handler);
        }
}

void
router_mount(Router* parent, const char* prefix, Router* child)
{
        if (parent->dispatcher && child->dispatcher &&
            parent->dispatcher == child->dispatcher) {
                parent->dispatcher->mount(parent->impl_data, prefix,
                                          child->impl_data);
                router_free(child);
        }
}

HandlerFunc
router_match_dispatcher(Router* router, const char* path, Request* req)
{
        if (router->dispatcher) {
                return router->dispatcher->match(router->impl_data, path, req);
        }
        return nullptr;
}

void
router_free(Router* router)
{
        if (router->dispatcher) {
                router->dispatcher->free(router->impl_data);
        } else {
                // if there is no dispatcher, this is a fallback for old arrays
                for (int i = 0; i < router->route_count; i++) {
                        free(router->patterns[i]);
                        regfree(&router->regex_patterns[i]);
                }
        }
        free(router);
}
