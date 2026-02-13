#ifndef HTTP_ROUTE_DISPATHER_H
#define HTTP_ROUTE_DISPATHER_H

#include <stddef.h>

#include "http/header.h"

struct RegexRouterData {
        struct Route* items;
        size_t len;
        size_t capacity;
};
typedef struct RegexRouterData RegexRouterData;

Router* router_create_regex(void);

#endif  // HTTP_ROUTE_DISPATHER_H
