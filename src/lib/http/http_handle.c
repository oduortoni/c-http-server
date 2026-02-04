#include "header.h"

int http_handle(Router* router, Client client) {
    char buffer[8192];

    puts("ONE");

    ssize_t bytes_read = read(client.socket, buffer, sizeof(buffer) - 1);

    puts("TWO");

    if (bytes_read < 0) {
        perror("read() failed");
        close(client.socket);
        return -1;
    }
    buffer[bytes_read] = '\0';

    Request* req = parse_http_request(buffer);
    if (!req) {
        perror("Failed to parse HTTP request");
        close(client.socket);
        return -1;
    }
    puts("THREE");

    // Prepare ResponseWriter
    ResponseWriter rw;
    InitResponseWriter(&rw);

    puts("FOUR");

    // Find and call the handler for the request path
    int success = 0;
    for (size_t i = 0; i < ARRAY_LEN(router->patterns) && router->patterns[i]; i++) {
        req->path_regex = &router->regex_patterns[i];
        if (regexec(req->path_regex, req->path, ARRAY_LEN(req->path_matches), req->path_matches, 0) == 0) {
            router->handlers[i](&rw, req);
            success = 1;
            break; // Stop searching once a matching handler is found
        }
    }
    if(!success) { // no path matched, invoke /404
        req->path_regex = NULL;
        for (size_t i = 0; i < ARRAY_LEN(router->patterns) && router->patterns[i]; i++) {
            if (strcmp("^/404$", router->patterns[i]) == 0) {
                router->handlers[i](&rw, req);
                break; // Stop searching once the 404 handler is found
            }
        }
    }
    puts("FIVE");

    // Build the HTTP response
    char response[8192];
    char* resp_ptr = BuildResponse(&rw);
    strncpy(response, resp_ptr, sizeof(response));

    // Send the response to the client
    ssize_t bytes_written = write(client.socket, response, strlen(response));
    if (bytes_written < 0) {
        perror("write() failed");
    }

    free_request(req);
    close(client.socket);
    return 0;
}
