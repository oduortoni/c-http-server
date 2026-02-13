#include "header.h"
#include "utils/logging/header.h"

/**
 * Starts a server that listens for incoming connections on the specified host
 * and port. The server will accept incoming connections and pass them to the
 * provided protocol handler function for processing. The `handle_protocol`
 * function is called for each accepted connection, allowing you to handle the
 * protocol-specific logic.
 *
 *  host - The host and port to listen on, in the format "host:port" (e.g.,
 * "localhost:8080").
 * handle_protocol - A function pointer to the protocol handler that will
 * process each accepted connection.
 * context - A pointer to a RequestContext structure that can be used to
 * pass additional information to the protocol handler.
 * returns 0 on success, or a non-zero value on failure
 */
int
net_serve(char* host, ProtocolHandler handle_protocol, RequestContext* context)
{
        int port;
        char head[50], tail[50];

        str_split(host, ':', head, tail);
        port = atoi(tail);
        if (port <= 0) {
                port = 9000;
        }

        int server_socket = net_listener(head, port);
        info("Serving requests on %s\n", host);

        while (1) {
                struct sockaddr_in client_addr;
                socklen_t client_addrlen = sizeof(client_addr);
                int client_conn =
                    accept(server_socket, (struct sockaddr*)&client_addr,
                           &client_addrlen);

                if (client_conn < 0) {
                        perror("accept() failed");
                        close(server_socket);
                        exit(1);
                }

                info("Accepted connection from %s:%d\n",
                       inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port));

                // TODO: avoid using a direct integer in favor of a variable
                // network layer, read bytes from socket
                char buffer[8192];
                ssize_t bytes_read =
                    read(client_conn, buffer, sizeof(buffer) - 1);
                if (bytes_read < 0) {
                        perror("read() failed");
                        close(client_conn);
                        continue;
                }
                buffer[bytes_read] = '\0';

                // protocol layer processes bytes (protocol-agnostic)
                ProtocolResponse response =
                    handle_protocol(context, buffer, bytes_read);

                // network layer writes bytes to socket
                if (response.status == 0 && response.data) {
                        ssize_t bytes_written =
                            write(client_conn, response.data, response.length);
                        if (bytes_written < 0) {
                                perror("write() failed");
                        }
                }

                // network layer: cleanup
                if (response.data) {
                        free(response.data);
                }
                close(client_conn);
        }

        return 0;
}
