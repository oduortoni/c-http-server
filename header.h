#ifndef _HEADER_H
#define _HEADER_H

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
    Header* headers;
    int (*Write)(char* p);
    int (*Read)(char* p);
};
typedef struct Request Request;

int (*HandlerFunc)(ResponseWriter *w, Request *r);
typedef HandlerFunc HandlerFunc;

struct Router {
    char patterns[50];
    HandlerFunc handlers[50];
};
typedef struct Router* Router;

struct Server {
    int (*ListenAndServe)(char *host, Router *router);
    int (*HandleFunc)(char *pattern, HandlerFunc *handler);
};
typedef struct Server Server;

#endif // _HEADER_H