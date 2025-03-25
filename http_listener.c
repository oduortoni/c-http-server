#include "header.h"

int listening_socket(char* host, int port);
void str_split(const char *input, char delimiter, char *head, char *tail);

int httpListener(char *host, Router router) {
    int port;
    char head[50], tail[50];

    str_split(host, ':', head, tail);
    port = atoi(tail);
    if (port <= 0) {
        port = 9000;
    }

    int server_socket = listening_socket(head, port);
    printf("Serving HTTP requests on %d\n", port);

    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientConn = accept(server_socket, (struct sockaddr*)&clientAddr, &clientAddrLen);

    if (clientConn < 0) {
        perror("accept() failed");
        close(server_socket);
        exit(1);
    }

    printf("Accepted connection from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

    // read/write data from client

    return clientConn;
}

void str_split(const char *input, char delimiter, char *head, char *tail) {
    const char *delimiter_pos = strchr(input, delimiter);
    if (delimiter_pos == NULL) {
        fprintf(stderr, "Error: Delimiter not found in the input string.\n");
        exit(1);
    }
    size_t head_len = delimiter_pos - input;

    strncpy(head, input, head_len);
    head[head_len] = '\0';

    strcpy(tail, delimiter_pos + 1);
}

int listening_socket(char* host, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0) {
        perror("socket() could not create a socket");
        exit(1);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    printf("Host: %s, Port: %d\n", host, port);

    // convert the host to a binary form and store it in address.sin_addr.s_addr
    if (inet_pton(AF_INET, host, &address.sin_addr) <= 0) {
        perror("inet_pton() failed to convert host address");
        exit(1);
    }

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
