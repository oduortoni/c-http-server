#ifndef _HTTP_HEADER_H
#define _HTTP_HEADER_H

#include "../net/header.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_HEADERS        20
#define MAX_METHOD_LEN     8
#define MAX_PATH_LEN       256
#define MAX_VERSION_LEN    16
#define MAX_HEADER_LEN     256
#define MAX_BODY_LEN       4096
#define MAX_STATUS_LEN     64
#define MAX_RESPONSE_LEN   8192
#define MAX_FORM_FIELDS 20
#define MAX_FIELD_LENGTH 256

#define ARRAY_LEN(a) ( sizeof(a) / sizeof(*(a)) )

struct FormField {
    char name[MAX_FIELD_LENGTH];
    char value[MAX_FIELD_LENGTH];
};
typedef struct FormField FormField;
struct FormData {
    FormField fields[MAX_FORM_FIELDS];
    int count;
};
typedef struct FormData FormData;

void url_decode(char* dest, const char* src);
void parse_form_data(const char* body, FormData* form_data);
const char* get_form_value(const FormData* form_data, const char* name);



typedef struct Header {
    char name[MAX_HEADER_LEN/2];
    char value[MAX_HEADER_LEN/2];
} Header;

typedef struct Request {
    char method[MAX_METHOD_LEN];
    char path[MAX_PATH_LEN];
    char version[MAX_VERSION_LEN];
    Header headers[MAX_HEADERS];
    int header_count;
    char body[MAX_BODY_LEN];
    size_t body_length;
} Request;

typedef struct ResponseWriter {
    int status_code;
    char status_text[MAX_STATUS_LEN];
    char version[MAX_VERSION_LEN];
    Header headers[MAX_HEADERS];
    int header_count;
    char body[MAX_BODY_LEN];
    size_t body_length;
    int (*Write)(struct ResponseWriter* rw, const char* data, size_t size);
    int (*WriteString)(struct ResponseWriter* rw, const char* str);
} ResponseWriter;
int response_write(ResponseWriter* rw, const char* data, size_t size);
int response_write_string(ResponseWriter* rw, const char* str);
void InitResponseWriter(ResponseWriter* rw);
void SetHeader(ResponseWriter* rw, const char* name, const char* value);
void SetStatus(ResponseWriter* rw, int code, const char* text);
char* BuildResponse(ResponseWriter* rw);

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

typedef int(*HandlerFunc)(ResponseWriter *w, Request *r);

struct Router {
    char* patterns[50];
    HandlerFunc handlers[50];
};
typedef struct Router Router;

typedef enum RouterStatus {
    ROUTER_OK = 0,
    ROUTER_FULL = -1,
    ROUTER_NOMEM = -2,
    ROUTER_INVALID = -3,
    ROUTER_DUPLICATE = -4
} RouterStatus;

struct HttpServer {
    int (*ListenAndServe)(char *host, Router *router);
    RouterStatus (*HandleFunc)(const char *pattern, HandlerFunc handler);
    Router *router;
};
typedef struct HttpServer HttpServer;

RouterStatus handleFunc(const char *pattern, HandlerFunc handler);
int listenAndServe(char *host, Router *router);
int http_handle(Router *router, Client client);
int http_handle_connection(RequestContext *context, Client client);

extern HttpServer http;

#endif // _HTTP_HEADER_H
