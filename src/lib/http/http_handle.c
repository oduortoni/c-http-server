#include "header.h"

// calls the pure function
// still knows about sockets - Next refactor is to move that to the net folder
int
http_handle(Router* router, Client client)
{
        char buffer[8192];

        puts("Handling client connection");

        ssize_t bytes_read = read(client.socket, buffer, sizeof(buffer) - 1);

        puts("Request received");

        if (bytes_read < 0) {
                perror("read() failed");
                close(client.socket);
                return -1;
        }
        buffer[bytes_read]    = '\0';

        // call pure function
        HttpResponse response = http_handle_pure_func(router, buffer);

        puts("Handler processed");

        // Send the response to the client
        ssize_t bytes_written =
            write(client.socket, response.data, response.length);
        if (bytes_written < 0) {
                perror("write() failed");
        }

        // Cleanup
        free(response.data);
        close(client.socket);
        return response.status;
}

HttpResponse
http_handle_pure_func(Router* router, const char* request_data)
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
