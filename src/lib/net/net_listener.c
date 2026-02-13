#include "header.h"

int
net_listener(char* host, int port)
{
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
                perror("socket() could not create a socket");
                exit(1);
        }

        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_port   = htons(port);

        // convert the host to a binary form and store it in
        // address.sin_addr.s_addr
        if (inet_pton(AF_INET, host, &address.sin_addr) <= 0) {
                perror("inet_pton() failed to convert host address");
                exit(1);
        }

        int reuse = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
        socklen_t addr_len = sizeof(address);
        if (bind(sock, (struct sockaddr*)&address, addr_len) < 0) {
                perror("bind() failed");
                close(sock);
                exit(1);
        }

        if (listen(sock, 5) < 0) {
                perror("listen() failed");
                close(sock);
                exit(1);
        }

        return sock;
}
