#ifndef _HEADER_H
#define _HEADER_H

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct Client {
        // char address[128];
        int socket;
};
typedef struct Client Client;

struct RequestContext {
        void* router;
};
typedef struct RequestContext RequestContext;

/*
 * A protocol specific handler.
 * A function that knows how to handle the specific protocol e.g http, etc.
 */
typedef int (*ProtocolHandler)(RequestContext* context, Client client);

int net_serve(char* host, ProtocolHandler handler, RequestContext* context);
int net_listener(char* host, int port);
void str_split(const char* input, char delimiter, char* head, char* tail);

#endif  // _HEADER_H