#include "header.h"
#include "utils/logging/header.h"
#include "utils/macros.h"

HttpResponse
http_handle(Router* router, const char* request_data)
{
        info("Processing HTTP request");

        Request* req = parse_http_request(request_data);
        if (!req) {
                error("Failed to parse HTTP request");
                return (HttpResponse){
                    .data   = strdup("HTTP/1.1 400 Bad Request\r\n\r\n"),
                    .length = 28,
                    .status = -1};
        }

        if (!req->path.data || req->path.size == 0) {
                error("Invalid request: empty path");
                return (HttpResponse){
                    .data   = strdup("HTTP/1.1 400 Bad Request\r\n\r\n"),
                    .length = 28,
                    .status = -1};
        }

        info("Looking for handler for '%.*s'", STRING_PRINT(req->path));

        ResponseWriter rw;
        InitResponseWriter(&rw);

        // Find and call handler
        HandlerFunc handler = router_match(router, req->path.data, req);

        // If no handler found, try 404
        if (handler == nullptr) {
                req->path_regex = nullptr;
                for (int i = 0; i < router->route_count; i++) {
                        if (strcmp("^/404$", router->patterns[i]) == 0) {
                                info("Using '%s' handler", router->patterns[i]);
                                handler = router->handlers[i];
                                break;
                        }
                }
        }

        // If no handler found, try 404
        if (handler == nullptr) {
                req->path_regex = nullptr;
                for (int i = 0; i < router->route_count && router->patterns[i];
                     i++) {
                        if (strcmp("^/404$", router->patterns[i]) == 0) {
                                info("Using '%s' handler", router->patterns[i]);
                                handler = router->handlers[i];
                                break;
                        }
                }
        }

        if (handler) {
                handler(&rw, req);
                info("Handler processed");
        } else {
                SetStatus(&rw, 404, "Not Found");
                rw.WriteString(&rw, "404 - Page not found");
        }

        char* response      = BuildResponse(&rw);
        size_t response_len = strlen(response);

        return (HttpResponse){
            .data = response, .length = response_len, .status = 0};
}
