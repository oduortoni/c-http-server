#ifndef _HTTP_HEADER_H
#define _HTTP_HEADER_H

#include <ctype.h>
#include <regex.h>

#include "net/header.h"
#include "utils/arena/header.h"
#include "utils/string/header.h"

#define MAX_HEADERS 20
#define MAX_METHOD_LEN 8
#define MAX_PATH_LEN 256
#define MAX_VERSION_LEN 16
#define MAX_HEADER_LEN 256
#define MAX_BODY_LEN 4096
#define MAX_STATUS_LEN 64
#define MAX_RESPONSE_LEN 8192
#define MAX_FORM_FIELDS 20
#define MAX_FIELD_LENGTH 256

typedef struct FormField FormField;
struct FormField {
        char name[MAX_FIELD_LENGTH];
        char value[MAX_FIELD_LENGTH];
};

typedef struct FormData FormData;
struct FormData {
        FormField fields[MAX_FORM_FIELDS];
        int count;
};

void url_decode(char* dest, struct String const src);
void parse_form_data(struct String body, FormData* form_data);
const char* get_form_value(const FormData* form_data, const char* name);
char const* get_mime_type(char const* filename);

typedef struct Header Header;
struct Header {
        struct String name;
        struct String value;
};

typedef struct Request Request;
struct Request {
        char method[MAX_METHOD_LEN];
        // HTTP doesn't specify maximum length
        // `path` required to be null terminated, thus
        // `path.data` holds pointer to cstr
        struct String path;
        char version[MAX_VERSION_LEN];
        // Dynamic array of headers.
        struct RequestHeaders {
                Header* items;
                size_t len;
                size_t capacity;
        } headers;
        struct String body;
        regex_t* path_regex;
        regmatch_t path_matches[20];
};

// TODO: add constructor/destructor functions
Request* parse_http_request(const char* req_bytes);
void free_request(Request* req);

typedef struct ResponseWriter ResponseWriter;
struct ResponseWriter {
        int status_code;
        char status_text[MAX_STATUS_LEN];
        char version[MAX_VERSION_LEN];
        Header headers[MAX_HEADERS];
        int header_count;
        char body[MAX_BODY_LEN];
        size_t body_length;
        // allocator for dynamic data, like headers
        struct Arena* allocator;
        int (*Write)(struct ResponseWriter* rw, const char* data, size_t size);
        int (*WriteString)(struct ResponseWriter* rw, const char* str);
};
int response_write(ResponseWriter* rw, const char* data, size_t size);
int response_write_string(ResponseWriter* rw, const char* str);
// TODO: add destructor function
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

typedef int (*HandlerFunc)(ResponseWriter* w, Request* r);

struct Router {
        char* patterns[50];
        regex_t regex_patterns[50];
        HandlerFunc handlers[50];
};
typedef struct Router Router;

typedef enum RouterStatus {
        ROUTER_OK            = 0,
        ROUTER_FULL          = -1,
        ROUTER_NOMEM         = -2,
        ROUTER_INVALID       = -3,
        ROUTER_DUPLICATE     = -4,
        ROUTER_INVALID_REGEX = -5,
} RouterStatus;

struct HttpServer {
        int (*ListenAndServe)(char* host, Router* router);
        RouterStatus (*HandleFunc)(const char* pattern, HandlerFunc handler);
        Router* router;
};
typedef struct HttpServer HttpServer;

typedef struct {
        char* data;
        size_t length;
        int status;
} HttpResponse;

RouterStatus handleFunc(const char* pattern, HandlerFunc handler);
int listenAndServe(char* host, Router* router);
HttpResponse http_handle(Router* router, const char* request_data);
ProtocolResponse http_handle_connection(RequestContext* context,
                                        const char* request_data,
                                        size_t request_len);

extern HttpServer http;

#endif  // _HTTP_HEADER_H
