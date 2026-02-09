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
 * Protocol response structure
 */
struct ProtocolResponse {
        char* data;
        size_t length;
        int status;
};
typedef struct ProtocolResponse ProtocolResponse;

/*
 * A protocol specific handler.
 * Takes request bytes, returns response bytes.
 * Protocol-agnostic: works with any protocol (HTTP, FTP, etc.)
 */
typedef ProtocolResponse (*ProtocolHandler)(RequestContext* context,
                                            const char* request_data,
                                            size_t request_len);

int net_serve(char* host, ProtocolHandler handler, RequestContext* context);
int net_listener(char* host, int port);
void str_split(const char* input, char delimiter, char* head, char* tail);

#endif  // _HEADER_H
