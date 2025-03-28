#ifndef _HEADER_H
#define _HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

struct Client {
    // char address[128];
    int socket;
};
typedef struct Client Client;

struct Processor {
    int (*handle_connection)(void*, Client);
    void *components;
};
typedef struct Processor Processor;

int serve(char *host, Processor processor);
int listener(char* host, int port);
void str_split(const char *input, char delimiter, char *head, char *tail);

#endif // _HEADER_H