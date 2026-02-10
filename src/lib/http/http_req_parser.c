#include <assert.h>
#include <stdint.h>

#include "header.h"
#include "utils/arena/header.h"
#include "utils/logging/header.h"
#include "utils/macros.h"

static thread_local struct Arena* arena = nullptr;
static thread_local struct String body  = {0};

char const* parse_http_method(Request* req, char const* p,
                              enum ParseStep* state);
char const* parse_http_path(Request* req, char const* p, enum ParseStep* state);
char const* parse_http_version(Request* req, char const* p,
                               enum ParseStep* state);
char const* parse_http_header_name(Request* req, char const* p,
                                   enum ParseStep* state, int* content_length);
char const* parse_http_header_value(Request* req, char const* p,
                                    enum ParseStep* state);
char const* parse_http_body(Request* req, char const* p, enum ParseStep* state,
                            int* content_length);

Request*
parse_http_request(const char* raw_request)
{
        if (!arena) {
                // 1Mb for headers should be enough for most cases
                // .. if body doesn't fit we can always just alloc more memory
                arena = malloc(sizeof *arena);
                if (!arena) {
                        error("failed to allocate memory for Request arena");
                        return nullptr;
                }
                size_t buf_len = 1024 * 1024;
                char* buf      = malloc(buf_len);
                if (!buf) {
                        error(
                            "failed to allocate memory for Request's arena "
                            "buffer");
                        return nullptr;
                }
                arena_init(arena, buf, buf_len);
        }
        assert(arena && "Arena MUST be initialized");
        assert(arena->buf && "Arena's buffer MUST be initialized");
        Request* req = arena_alloc(arena, sizeof *req);
        if (!req) {
                // TODO: 500 Internal Server Error
                error("failed to allocate memory for request");
                return nullptr;
        }

        enum ParseStep state = PARSE_METHOD;
        const char* p        = raw_request;
        int content_length   = 0;

        while (p && *p && state != PARSE_COMPLETE && state != PARSE_ERROR) {
                switch (state) {
                case PARSE_METHOD:
                        p = parse_http_method(req, p, &state);
                        break;
                case PARSE_PATH:
                        p = parse_http_path(req, p, &state);
                        break;
                case PARSE_VERSION:
                        p = parse_http_version(req, p, &state);
                        break;
                case PARSE_HEADER_NAME:
                        p = parse_http_header_name(req, p, &state,
                                                   &content_length);
                        break;
                case PARSE_HEADER_VALUE:
                        p = parse_http_header_value(req, p, &state);
                        break;
                case PARSE_BODY:
                        p = parse_http_body(req, p, &state, &content_length);
                        break;
                default:
                        state = PARSE_ERROR;
                        break;
                }
        }

        if (state == PARSE_ERROR) {
                free_request(req);
                return NULL;
        }

        return req;
}

void
free_request([[maybe_unused]] Request* req)
{
        arena_free_all(arena);
}

char const*
parse_http_method(Request* req, char const* p, enum ParseStep* state)
{
        char* method_ptr = req->method;
        while (*p && !isspace(*p)) {
                if (method_ptr - req->method < MAX_METHOD_LEN - 1) {
                        *method_ptr++ = *p;
                }
                p++;
        }
        *method_ptr = '\0';

        if (*p) p++;  // skip space
        debug("Finished parsing HTTP method: '%s'", req->method);
        *state = PARSE_PATH;

        return p;
}

char const*
parse_http_path(Request* req, char const* p, enum ParseStep* state)
{
        req->path = (typeof(req->path)){0};

        while (*p && !isspace(*p)) {
                char* path_ptr = arena_alloc_array(arena, 1, path_ptr);
                if (req->path.data == nullptr) {
                        req->path.data = path_ptr;
                }
                // TODO: If a request exceeds a server's limit,
                // it typically returns an HTTP 414 URI Too Long
                // status code.
                if (path_ptr != nullptr) {
                        *path_ptr = *p;
                        req->path.size++;
                } else {
                        error("path too long");
                        *state = PARSE_ERROR;
                        return nullptr;
                }
                p++;
        }

        // `regexec` requires null terminated string
        // .. add null terminator to saticfy cstr but do not
        //    change the size of the actual string
        if (!arena_alloc_array(arena, 1, req->path.data)) {
                // TODO: 414 status code
                error("path too long");
                *state = PARSE_ERROR;
                return nullptr;
        }

        if (*p) p++;  // skip space
        *state = PARSE_VERSION;

        debug("Finished parsing PATH: %s", req->path.data);

        return p;
}

char const*
parse_http_version(Request* req, char const* p, enum ParseStep* state)
{
        char* version_ptr = req->version;
        while (*p && *p != '\r' && *p != '\n') {
                if (version_ptr - req->version < MAX_VERSION_LEN - 1) {
                        *version_ptr++ = *p;
                }
                p++;
        }
        *version_ptr = '\0';

        // Skip CRLF
        if (*p == '\r') p++;
        if (*p == '\n') p++;

        *state = PARSE_HEADER_NAME;
        debug("Finished parsing VERSION %s", req->version);

        return p;
}

char const*
parse_http_header_name(Request* req, char const* p, enum ParseStep* state,
                       int* content_length)
{
        // end of header section reached
        if (*p == '\r' || *p == '\n') {
                if (*p == '\r') p++;
                if (*p == '\n') p++;

                // Check for Content-Length
                for (size_t i = 0; i < req->headers.len; i++) {
                        auto header          = &req->headers.items[i];
                        char ContentLength[] = "Content-Length";
                        if (header->name.size + 1 != sizeof(ContentLength)) {
                                continue;
                        }

                        if (!strncasecmp(header->name.data, ContentLength,
                                         header->name.size)) {
                                *content_length = atoi(header->value.data);
                        }
                }

                // TODO: check for Transfer-Encoding: chunked

                *state = *content_length > 0 ? PARSE_BODY : PARSE_COMPLETE;
                debug(
                    "Finished parsing headers. Content-Length: "
                    "%d, p: '%s'",
                    *content_length, p);

                return p;
        }

        // redirect allocation operations to the arena
#define malloc(size) arena_alloc(arena, size)
#define realloc(array, size) arena_resize(arena, array, size / 2, size)
        da_append(req->headers, (typeof(*req->headers.items)){0});
#undef malloc
#undef realloc

        // got an allocation error while extending dynamic array
        if (!req->headers.items) {
                // TODO: 500 Internal Server Error
                error("can't extend req->headers");
                *state = PARSE_ERROR;
                return nullptr;
        }

        auto current_header = &req->headers.items[req->headers.len - 1];

        while (*p && *p != ':' && !isspace(*p)) {
                char* current_name = arena_alloc_array(arena, 1, current_name);
                if (current_header->name.data == nullptr) {
                        current_header->name.data = current_name;
                }
                if (current_name) {
                        *current_name = *p;
                        current_header->name.size++;
                } else {
                        // TODO: 500 Internal Server Error
                        error("header name too long");
                        *state = PARSE_ERROR;
                        return nullptr;
                }
                p++;
        }

        // Skip colon and whitespace
        while (*p && (*p == ':' || isspace(*p))) p++;

        *state = PARSE_HEADER_VALUE;

        return p;
}
char const*
parse_http_header_value(Request* req, char const* p, enum ParseStep* state)
{
        // current_header->value has been zero initialized by
        // `da_append` macro

        auto current_header = &req->headers.items[req->headers.len - 1];

        while (*p && *p != '\r' && *p != '\n') {
                char* current_value =
                    arena_alloc_array(arena, 1, current_value);
                if (current_header->value.data == nullptr) {
                        current_header->value.data = current_value;
                }
                if (current_value != nullptr) {
                        *current_value = *p;
                        current_header->value.size++;
                } else {
                        // TODO: 500 Internal Server Error
                        error("header value is too long");
                        *state = PARSE_ERROR;
                        return nullptr;
                }
                p++;
        }

        // Skip CRLF
        if (*p == '\r') p++;
        if (*p == '\n') p++;

        *state = PARSE_HEADER_NAME;

        return p;
}

char const*
parse_http_body(Request* req, char const* p, enum ParseStep* state,
                int* content_length)
{
        size_t bytes_remaining = strlen(p);
        assert(*content_length >= 0);
        size_t bytes_to_copy = ((size_t)content_length < bytes_remaining)
                                   ? (size_t)content_length
                                   : bytes_remaining;

        if (!body.size) {
                body.data = malloc(bytes_to_copy);
                body.size = bytes_to_copy;
        }
        if (bytes_to_copy > body.size) {
                body.size = bytes_to_copy;
                body.data = realloc(body.data, body.size);
        }
        if (!body.data) {
                // TODO: 500 Internal Server Error
                error("can't allocate memory for body");
                *state = PARSE_ERROR;
                return nullptr;
        }

        req->body = (struct String){
            .data = body.data,
            .size = bytes_to_copy,
        };
        memcpy(req->body.data, p, bytes_to_copy);
        p += bytes_to_copy;

        *state = PARSE_COMPLETE;

        debug("Finished parsing body: '%.*s'", STRING_PRINT(req->body));

        return p;
}
