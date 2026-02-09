#include "header.h"
#include "utils/logging/header.h"
#include "utils/macros.h"

int
http_handle(Router* router, Client client)
{
        // TODO: replace with string builder
        char buffer[8192];

        info("Handling client connection");

        ssize_t bytes_read = read(client.socket, buffer, sizeof(buffer) - 1);

        info("Request received");

        if (bytes_read < 0) {
                error("read() failed");
                perror("read() failed");  // TODO: format errno
                close(client.socket);
                return -1;
        }
        buffer[bytes_read] = '\0';

        Request* req       = parse_http_request(buffer);
        if (!req) {
                error("Failed to parse HTTP request");
                perror("Failed to parse HTTP request");  // TODO: format errno
                close(client.socket);
                return -1;
        }

        info("Request parsed");

        // Prepare ResponseWriter
        ResponseWriter rw;
        InitResponseWriter(&rw);

        info("Looking for handler for '%.*s'", STRING_PRINT(req->path));

        // Find and call the handler for the request path
        HandlerFunc handler = nullptr;
        for (size_t i = 0;
             i < ARRAY_LEN(router->patterns) && router->patterns[i]; i++) {
                req->path_regex = &router->regex_patterns[i];
                if (!regexec(req->path_regex, req->path.data,
                             ARRAY_LEN(req->path_matches), req->path_matches,
                             0)) {
                        info("Using '%s' hander", router->patterns[i]);
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
                                info("Using '%s' hander", router->patterns[i]);
                                handler = router->handlers[i];
                                break;  // Stop searching once the 404 handler
                                        // is found
                        }
                }
        }
        if (handler) {
                handler(&rw, req);

                info("Handler processed");

                // Build the HTTP response
                // TODO: use string builder and return string view
                char* response = BuildResponse(&rw);

                // Send the response to the client
                // TODO: `strlen` will not work for binary data, e.g. pdf files
                ssize_t bytes_written =
                    write(client.socket, response, strlen(response));
                if (bytes_written < 0) {
                        perror("write() failed");  // TODO: format errno
                }
        }

        arena_free_all(rw.allocator);
        free_request(req);
        close(client.socket);
        return 0;
}
