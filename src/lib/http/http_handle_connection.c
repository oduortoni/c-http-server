#include "header.h"

int
http_handle_connection(RequestContext* context, Client client)
{
        Router* router = (Router*)context->router;
        char buffer[8192];

        // Network layer: Read from socket
        ssize_t bytes_read = read(client.socket, buffer, sizeof(buffer) - 1);
        if (bytes_read < 0) {
                perror("read() failed");
                close(client.socket);
                return -1;
        }
        buffer[bytes_read]    = '\0';

        // HTTP layer: Process request (pure function)
        HttpResponse response = http_handle(router, buffer);

        // Network layer: Write to socket
        ssize_t bytes_written =
            write(client.socket, response.data, response.length);
        if (bytes_written < 0) {
                perror("write() failed");
        }

        // Network layer: Cleanup
        free(response.data);
        close(client.socket);

        return response.status;
}
