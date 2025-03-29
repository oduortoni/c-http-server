#ifndef _HTTP_HEADER_H
#define _HTTP_HEADER_H

#include "../net/header.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct ResponseWriter {
    int (*Write)(char* p);
    int (*Read)(char* p);
};
typedef struct ResponseWriter ResponseWriter;

#define MAX_HEADERS 20
#define MAX_METHOD_LEN 8
#define MAX_PATH_LEN 256
#define MAX_VERSION_LEN 16
#define MAX_HEADER_NAME_LEN 120
#define MAX_HEADER_VALUE_LEN 120
#define MAX_BODY_LEN 4096
#define MAX_RESPONSE_LEN 8092

struct Header {
    char name[MAX_HEADER_NAME_LEN];
    char value[MAX_HEADER_VALUE_LEN];
};
typedef struct Header Header;

struct Request {
    char method[MAX_METHOD_LEN];
    char path[MAX_PATH_LEN];
    char version[MAX_VERSION_LEN];
    struct Header headers[MAX_HEADERS];
    int header_count;
    char body[MAX_BODY_LEN];
    size_t body_length;
    int (*Write)(char* p); // match io.Writer
    int (*Read)(char* p); // match io.Reader
};
typedef struct Request Request;

enum ParseState {
    PARSE_METHOD,
    PARSE_PATH,
    PARSE_VERSION,
    PARSE_HEADER_NAME,
    PARSE_HEADER_VALUE,
    PARSE_BODY,
    PARSE_COMPLETE,
    PARSE_ERROR
};

Request* parse_http_request(const char* req_bytes);
void free_request(Request* req);

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