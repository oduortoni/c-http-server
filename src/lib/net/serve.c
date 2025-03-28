#include "header.h"

int serve(char *host, Processor processor) {
    int port;
    char head[50], tail[50];

    str_split(host, ':', head, tail);
    port = atoi(tail);
    if (port <= 0) {
        port = 9000;
    }

    int server_socket = listener(head, port);
    printf("Serving HTTP requests on %d\n", port);
    
    while(1) {
      struct sockaddr_in client_addr;
      socklen_t client_addrlen = sizeof(client_addr);
      int client_conn = accept(server_socket, (struct sockaddr*)&client_addr, &client_addrlen);

      if (client_conn < 0) {
        perror("accept() failed");
        close(server_socket);
        exit(1);
      }

      printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

      Client client = {client_conn};
      
      processor.handle_connection(processor.components, client);
    }

    return 0;
}
