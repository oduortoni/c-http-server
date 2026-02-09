#include "header.h"

HttpResponse
http_handle(Router* router, const char* request_data)
{
        Request* req = parse_http_request(request_data);
        if (!req) {
                return (HttpResponse){
                    .data   = strdup("HTTP/1.1 400 Bad Request\r\n\r\n"),
                    .length = 28,
                    .status = -1};
        }

        ResponseWriter rw;
        InitResponseWriter(&rw);

        // find and call handler
        HandlerFunc handler = nullptr;
        for (size_t i = 0;
             i < ARRAY_LEN(router->patterns) && router->patterns[i]; i++) {
                req->path_regex = &router->regex_patterns[i];
                if (regexec(req->path_regex, req->path,
                            ARRAY_LEN(req->path_matches), req->path_matches,
                            0) == 0) {
                        handler = router->handlers[i];
                        break;
                }
        }

        // if no handler found, try go to 404
        if (handler == nullptr) {
                req->path_regex = nullptr;
                for (size_t i = 0;
                     i < ARRAY_LEN(router->patterns) && router->patterns[i];
                     i++) {
                        if (strcmp("^/404$", router->patterns[i]) == 0) {
                                handler = router->handlers[i];
                                break;
                        }
                }
        }

        if (handler) {
                handler(&rw, req);
        } else {
                SetStatus(&rw, 404, "Not Found");
                rw.WriteString(&rw, "404 - Page not found");
        }

        char* response      = BuildResponse(&rw);
        size_t response_len = strlen(response);

        free_request(req);

        return (HttpResponse){
            .data = response, .length = response_len, .status = 0};
}
