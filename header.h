#ifndef _HEADER_H
#define _HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

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

struct Client {
    // char address[128];
    int socket;
};
typedef struct Client Client;

struct Router {
    char patterns[50];
    HandlerFunc handlers[50];
};
typedef struct Router* Router;

struct HttpServer {
    int (*ListenAndServe)(char *host, Router router);
    int (*HandleFunc)(char *pattern, HandlerFunc handler);
};
typedef struct HttpServer HttpServer;

int httpListener(char *host, Router router);
int handleFunc(char *pattern, HandlerFunc handler);
int handleConnection(Router router, Client client);

extern HttpServer http;

#endif // _HEADER_H