#include "header.h"
#include "http/header.h"
#include "utils/macros.h"

HandlerFunc
regex_match(void* impl_data, const char* path, Request* req)
{
        printf("Search for route '%s'\n", path);

        RegexRouterData* data = (RegexRouterData*)impl_data;
        for (size_t i = 0; i < data->len; i++) {
                req->path_regex = &data->items[i].compiled_pattern;
                if (!regexec(req->path_regex, path,
                             ARRAY_LEN(req->path_matches), req->path_matches,
                             0)) {
                        return data->items[i].handler;
                }
        }
        return nullptr;
}

/* FIX ISSUES*/
static void
regex_add_route(void* impl_data, const char* pattern, HandlerFunc handler)
{
        RegexRouterData* data_ptr = (RegexRouterData*)impl_data;
        RegexRouterData data;

        data.items         = data_ptr->items;
        data.capacity      = data_ptr->capacity;
        data.len           = data_ptr->len;

        struct Route route = {.pattern = strdup(pattern), .handler = handler};

        regcomp(&route.compiled_pattern, pattern, REG_EXTENDED);

        da_append(data, route);

        data_ptr->items    = data.items;
        data_ptr->capacity = data.capacity;
        data_ptr->len      = data.len;
}

static void
regex_mount(void* parent_data, const char* prefix, void* child_data)
{
        RegexRouterData* child = (RegexRouterData*)child_data;

        for (size_t i = 0; i < child->len; i++) {
                char* new_pattern = malloc(strlen(prefix) +
                                           strlen(child->items[i].pattern) + 1);
                strcpy(new_pattern, prefix);
                strcat(new_pattern, child->items[i].pattern);
                regex_add_route(parent_data, new_pattern,
                                child->items[i].handler);
                free(new_pattern);
        }
}

static void
regex_free(void* impl_data)
{
        RegexRouterData* data = (RegexRouterData*)impl_data;
        for (size_t i = 0; i < data->len; i++) {
                free(data->items[i].pattern);
                regfree(&data->items[i].compiled_pattern);
        }
        free(data->items);
        free(data);
}

// initialize the regex dispatcher with member fields
static const Dispatcher regex_dispatcher = {.match     = regex_match,
                                            .add_route = regex_add_route,
                                            .mount     = regex_mount,
                                            .free      = regex_free};

Router*
router_create_regex(void)
{
        Router* router        = calloc(1, sizeof(Router));
        RegexRouterData* data = calloc(1, sizeof(RegexRouterData));
        router->dispatcher    = &regex_dispatcher;
        router->impl_data     = data;
        return router;
}
