#include "header.h"
#include "http/header.h"

Router*
BlogRouter()
{
        Router* router = router_create_regex();
        router_add(router, "^/blog$", BlogIndex);
        return router;
}
