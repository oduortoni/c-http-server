#include <assert.h>
#include <stdint.h>

#include "header.h"
#include "utils/arena/header.h"
#include "utils/logging/header.h"
#include "utils/macros.h"

static thread_local struct Arena* arena = nullptr;
static thread_local struct String body  = {0};

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
        if (!req) return nullptr;

        memset(req, 0, sizeof *req);

        enum ParseState state  = PARSE_METHOD;
        const char* p          = raw_request;
        Header* current_header = nullptr;
        char* current_name     = nullptr;
        char* current_value    = nullptr;
        int content_length     = 0;

        while (*p && state != PARSE_COMPLETE && state != PARSE_ERROR) {
                switch (state) {
                case PARSE_METHOD: {
                        char* method_ptr = req->method;
                        while (*p && !isspace(*p)) {
                                if (method_ptr - req->method <
                                    MAX_METHOD_LEN - 1) {
                                        *method_ptr++ = *p;
                                }
                                p++;
                        }
                        *method_ptr = '\0';
                        if (*p) p++;  // skip space
                        debug("Finished parsing HTTP method: '%s'",
                              req->method);
                        state = PARSE_PATH;
                        break;
                }

                case PARSE_PATH: {
                        // allocate 1 byte in the arena
                        req->path.data = arena_alloc(arena, 1);
                        char* path_ptr = req->path.data;
                        int max_len    = arena->buf_len - arena->prev_offset;
                        while (*p && !isspace(*p)) {
                                // TODO: If a request exceeds a server's limit,
                                // it typically returns an HTTP 414 URI Too Long
                                // status code.
                                if (path_ptr - req->path.data < max_len) {
                                        *path_ptr++ = *p;
                                } else {
                                        error("path too long");
                                        state = PARSE_ERROR;
                                        break;
                                }
                                p++;
                        }
                        req->path.size = path_ptr - req->path.data;
                        // `regexec` requires null terminated string
                        req->path.data[req->path.size] = '\0';
                        // claim rest of the required size as used
                        arena->curr_offset += req->path.size;
                        if (*p) p++;  // skip space
                        state = PARSE_VERSION;

                        debug("Finished parsing PATH: %s", req->path.data);
                        break;
                }

                case PARSE_VERSION: {
                        char* version_ptr = req->version;
                        while (*p && *p != '\r' && *p != '\n') {
                                if (version_ptr - req->version <
                                    MAX_VERSION_LEN - 1) {
                                        *version_ptr++ = *p;
                                }
                                p++;
                        }
                        *version_ptr = '\0';

                        // Skip CRLF
                        if (*p == '\r') p++;
                        if (*p == '\n') p++;

                        state = PARSE_HEADER_NAME;
                        debug("Finished parsing VERSION %s", req->version);
                        break;
                }

                case PARSE_HEADER_NAME: {
                        if (*p == '\r' || *p == '\n') {
                                if (*p == '\r') p++;
                                if (*p == '\n') p++;

                                // Check for Content-Length
                                for (size_t i = 0; i < req->headers.len; i++) {
                                        auto header = &req->headers.items[i];
                                        char ContentLength[] = "Content-Length";
                                        if (header->name.size + 1 !=
                                            sizeof(ContentLength)) {
                                                continue;
                                        }

                                        if (!strncasecmp(header->name.data,
                                                         ContentLength,
                                                         header->name.size)) {
                                                content_length =
                                                    atoi(header->value.data);
                                        }
                                }

                                // TODO: check for Transfer-Encoding: chunked

                                state = content_length > 0 ? PARSE_BODY
                                                           : PARSE_COMPLETE;
                                debug(
                                    "Finished parsing headers. Content-Length: "
                                    "%d, p: '%s'",
                                    content_length, p);
                                continue;
                        }

                        // redirect allocation operations to the arena
#define malloc(size) arena_alloc(arena, size)
#define realloc(array, size) arena_resize(arena, array, size / 2, size)
                        da_append(req->headers, (struct Header){0});
#undef malloc
#undef realloc

                        // got an allocation error while extending dynamic array
                        if (!req->headers.items) {
                                // TODO: 500 Internal Server Error
                                error("can't extend req->headers");
                                state = PARSE_ERROR;
                                break;
                        }

                        current_header =
                            &req->headers.items[req->headers.len - 1];

                        // allocate 1 byte to mark starting point of header name
                        current_header->name.data = arena_alloc(arena, 1);
                        size_t max_len = arena->buf_len - arena->prev_offset;
                        if (!current_header->name.data) {
                                // TODO: 500 Internal Server Error
                                error(
                                    "can't allocate current_header->name.data");
                                state = PARSE_ERROR;
                                break;
                        }

                        current_name = current_header->name.data;
                        while (*p && *p != ':' && !isspace(*p)) {
                                if ((uintptr_t)current_name -
                                        (uintptr_t)current_header->name.data <
                                    max_len) {
                                        *current_name++ = *p;
                                } else {
                                        // TODO: 500 Internal Server Error
                                        error("header name too long");
                                        state = PARSE_ERROR;
                                        break;
                                }
                                p++;
                        }
                        current_header->name.size =
                            current_name - current_header->name.data;
                        // claim the rest of the required size as used
                        arena->curr_offset += current_header->name.size;

                        // Skip colon and whitespace
                        while (*p && (*p == ':' || isspace(*p))) p++;

                        state = PARSE_HEADER_VALUE;
                        break;
                }

                case PARSE_HEADER_VALUE: {
                        current_header->value.data = arena_alloc(arena, 1);
                        int max_len = arena->buf_len - arena->prev_offset;
                        if (!current_header->value.data) {
                                // TODO: 500 Internal Server Error
                                error(
                                    "can't allocate "
                                    "current_header->value.data");
                                state = PARSE_ERROR;
                                break;
                        }
                        current_value = current_header->value.data;
                        while (*p && *p != '\r' && *p != '\n') {
                                if (current_value - current_header->value.data <
                                    max_len) {
                                        *current_value++ = *p;
                                } else {
                                        // TODO: 500 Internal Server Error
                                        error("header value is too long");
                                        state = PARSE_ERROR;
                                        break;
                                }
                                p++;
                        }
                        current_header->value.size =
                            current_value - current_header->value.data;
                        // claim the rest of the required size as used
                        arena->curr_offset += current_header->value.size;

                        // Skip CRLF
                        if (*p == '\r') p++;
                        if (*p == '\n') p++;

                        state = PARSE_HEADER_NAME;
                        break;
                }

                case PARSE_BODY: {
                        size_t bytes_remaining = strlen(p);
                        assert(content_length >= 0);
                        size_t bytes_to_copy =
                            ((size_t)content_length < bytes_remaining)
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
                                state = PARSE_ERROR;
                                break;
                        }

                        req->body = (struct String){
                            .data = body.data,
                            .size = bytes_to_copy,
                        };
                        memcpy(req->body.data, p, bytes_to_copy);
                        p += bytes_to_copy;

                        state = PARSE_COMPLETE;

                        debug("Finished parsing body: '%.*s'",
                              STRING_PRINT(req->body));
                        break;
                }

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
