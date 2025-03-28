#ifndef _HTTP_HEADER_H
#define _HTTP_HEADER_H

#include "../net/header.h"

struct ResponseWriter {
    int (*Write)(char* p);
    int (*Read)(char* p);
};
typedef struct ResponseWriter ResponseWriter;

struct Header {
    char Name[120];
    char Content[120];
};
typedef struct Header* Header;

struct Request {
    Header headers[10];
    int (*Write)(char* p);
    int (*Read)(char* p);
};
typedef struct Request Request;

typedef int(*HandlerFunc)(ResponseWriter w, Request r);

struct Router {
    char* patterns[50];
    HandlerFunc handlers[50];
};
typedef struct Router Router;

struct HttpServer {
    int (*ListenAndServe)(char *host, Router router);
    int (*HandleFunc)(char *pattern, HandlerFunc handler);
};
typedef struct HttpServer HttpServer;

int handleFunc(char *pattern, HandlerFunc handler);
int listenAndServe(char *host, Router router);
int handle(Router router, Client client);
int handle_connection(void *components, Client client);

extern HttpServer http;

#endif // _HTTP_HEADER_H