#include "header.h"

int handleConnection(Router router, Client client) {
    char buffer[1024];

    ssize_t bytes_read = read(client.socket, buffer, sizeof(buffer) - 1);

    if (bytes_read < 0) {
        perror("read() failed");
        close(client.socket);
        return -1;
    }
    buffer[bytes_read] = '\0';

    printf("Received from client: %s\n", buffer);

    // response back to the client
    const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, world!";
    ssize_t bytes_written = write(client.socket, response, strlen(response));

    if (bytes_written < 0) {
        perror("write() failed");
        close(client.socket);
        return -1;
    }

    close(client.socket);

    return 0;
}