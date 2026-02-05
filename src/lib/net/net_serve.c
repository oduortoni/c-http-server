#include "header.h"

/**
 * Starts a server that listens for incoming connections on the specified host
 * and port. The server will accept incoming connections and pass them to the
 * provided protocol handler function for processing. The `handle_protocol`
 * function is called for each accepted connection, allowing you to handle the
 * protocol-specific logic.
 *
 * @param host The host and port to listen on, in the format "host:port" (e.g.,
 * "localhost:8080").
 * @param handle_protocol A function pointer to the protocol handler that will
 * process each accepted connection.
 * @param context A pointer to a RequestContext structure that can be used to
 * pass additional information to the protocol handler.
 * @return 0 on success, or a non-zero value on failure
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
        printf("Serving requests on %d\n", port);

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

                printf("Accepted connection from %s:%d\n",
                       inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port));

                Client client = {client_conn};

                handle_protocol(context, client);
        }

        return 0;
}
