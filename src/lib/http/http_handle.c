#include "header.h"

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
        buffer[bytes_read] = '\0';

        Request* req       = parse_http_request(buffer);
        if (!req) {
                perror("Failed to parse HTTP request");
                close(client.socket);
                return -1;
        }

        puts("Request parsed");

        // Prepare ResponseWriter
        ResponseWriter rw;
        InitResponseWriter(&rw);

        printf("Looking for handler for '%s'\n", req->path);

        // Find and call the handler for the request path
        HandlerFunc handler = nullptr;
        for (size_t i = 0;
             i < ARRAY_LEN(router->patterns) && router->patterns[i]; i++) {
                req->path_regex = &router->regex_patterns[i];
                if (regexec(req->path_regex, req->path,
                            ARRAY_LEN(req->path_matches), req->path_matches,
                            0) == 0) {
                        printf("Using '%s' hander\n", router->patterns[i]);
                        handler = router->handlers[i];
                        break;     // Stop searching once a matching handler is
                                   // found
                }
        }
        if (handler == nullptr) {  // no path matched, invoke /404
                req->path_regex = nullptr;
                for (size_t i = 0;
                     i < ARRAY_LEN(router->patterns) && router->patterns[i];
                     i++) {
                        if (strcmp("^/404$", router->patterns[i]) == 0) {
                                printf("Using '%s' hander\n",
                                       router->patterns[i]);
                                handler = router->handlers[i];
                                break;  // Stop searching once the 404 handler
                                        // is found
                        }
                }
        }
        if (handler) {
                handler(&rw, req);

                puts("Handler processed");

                // Build the HTTP response
                char* response = BuildResponse(&rw);

                // Send the response to the client
                // TODO: `strlen` will not work for binary data, e.g. pdf files
                ssize_t bytes_written =
                    write(client.socket, response, strlen(response));
                if (bytes_written < 0) {
                        perror("write() failed");
                }
        }

        free_request(req);
        close(client.socket);
        return 0;
}

HttpResponse
http_handle_pure_func(Router* router, const char* request_data,
                      size_t request_len)
{
        return (HttpResponse){
            .data   = strdup("HTTP/1.1 501 Not Implemented\r\n\r\n"),
            .length = 35,
            .status = 0};
}
