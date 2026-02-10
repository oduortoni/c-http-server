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
        if (!req) {
                // TODO: 500 Internal Server Error
                error("failed to allocate memory for request");
                return nullptr;
        }

        struct ParseState state = {
            .req            = req,
            .step           = PARSE_METHOD,
            .p              = raw_request,
            .content_length = 0,
        };

        while (state.p && *state.p && state.step != PARSE_COMPLETE &&
               state.step != PARSE_ERROR) {
                switch (state.step) {
                case PARSE_METHOD:
                        parse_http_method(&state);
                        break;
                case PARSE_PATH:
                        parse_http_path(&state);
                        break;
                case PARSE_VERSION:
                        parse_http_version(&state);
                        break;
                case PARSE_HEADER_NAME:
                        parse_http_header_name(&state);
                        break;
                case PARSE_HEADER_VALUE:
                        parse_http_header_value(&state);
                        break;
                case PARSE_BODY:
                        parse_http_body(&state);
                        break;
                default:
                        state.step = PARSE_ERROR;
                        break;
                }
        }

        if (state.step == PARSE_ERROR) {
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

void
parse_http_method(struct ParseState* state)
{
        char* method_ptr = state->req->method;
        while (*state->p && !isspace(*state->p)) {
                if (method_ptr - state->req->method < MAX_METHOD_LEN - 1) {
                        *method_ptr++ = *state->p;
                }
                state->p++;
        }
        *method_ptr = '\0';

        if (*state->p) state->p++;  // skip space
        debug("Finished parsing HTTP method: '%s'", state->req->method);
        state->step = PARSE_PATH;
}

void
parse_http_path(struct ParseState* state)
{
        state->req->path = (typeof(state->req->path)){0};

        while (*state->p && !isspace(*state->p)) {
                char* path_ptr = arena_alloc_array(arena, 1, path_ptr);
                if (state->req->path.data == nullptr) {
                        state->req->path.data = path_ptr;
                }
                // TODO: If a request exceeds a server's limit,
                // it typically returns an HTTP 414 URI Too Long
                // status code.
                if (path_ptr != nullptr) {
                        *path_ptr = *state->p;
                        state->req->path.size++;
                } else {
                        error("path too long");
                        state->step = PARSE_ERROR;
                        return;
                }
                state->p++;
        }

        // `regexec` requires null terminated string
        // .. add null terminator to saticfy cstr but do not
        //    change the size of the actual string
        if (!arena_alloc_array(arena, 1, state->req->path.data)) {
                // TODO: 414 status code
                error("path too long");
                state->step = PARSE_ERROR;
        }

        if (*state->p) state->p++;  // skip space
        state->step = PARSE_VERSION;

        debug("Finished parsing PATH: %s", state->req->path.data);
}

void
parse_http_version(struct ParseState* state)
{
        char* version_ptr = state->req->version;
        while (*state->p && *state->p != '\r' && *state->p != '\n') {
                if (version_ptr - state->req->version < MAX_VERSION_LEN - 1) {
                        *version_ptr++ = *state->p;
                }
                state->p++;
        }
        *version_ptr = '\0';

        // Skip CRLF
        if (*state->p == '\r') state->p++;
        if (*state->p == '\n') state->p++;

        state->step = PARSE_HEADER_NAME;
        debug("Finished parsing VERSION %s", state->req->version);
}

void
parse_http_header_name(struct ParseState* state)
{
        // end of header section reached
        if (*state->p == '\r' || *state->p == '\n') {
                if (*state->p == '\r') state->p++;
                if (*state->p == '\n') state->p++;

                // Check for Content-Length
                for (size_t i = 0; i < state->req->headers.len; i++) {
                        auto header          = &state->req->headers.items[i];
                        char ContentLength[] = "Content-Length";
                        if (header->name.size + 1 != sizeof(ContentLength)) {
                                continue;
                        }

                        if (!strncasecmp(header->name.data, ContentLength,
                                         header->name.size)) {
                                state->content_length =
                                    atoi(header->value.data);
                        }
                }

                // TODO: check for Transfer-Encoding: chunked

                state->step =
                    state->content_length > 0 ? PARSE_BODY : PARSE_COMPLETE;
                debug(
                    "Finished parsing headers. Content-Length: "
                    "%d, p: '%s'",
                    state->content_length, state->p);
        }

        // redirect allocation operations to the arena
#define malloc(size) arena_alloc(arena, size)
#define realloc(array, size) arena_resize(arena, array, size / 2, size)
        da_append(state->req->headers, (typeof(*state->req->headers.items)){0});
#undef malloc
#undef realloc

        // got an allocation error while extending dynamic array
        if (!state->req->headers.items) {
                // TODO: 500 Internal Server Error
                error("can't extend req->headers");
                state->step = PARSE_ERROR;
        }

        auto current_header =
            &state->req->headers.items[state->req->headers.len - 1];

        while (*state->p && *state->p != ':' && !isspace(*state->p)) {
                char* current_name = arena_alloc_array(arena, 1, current_name);
                if (current_header->name.data == nullptr) {
                        current_header->name.data = current_name;
                }
                if (current_name) {
                        *current_name = *state->p;
                        current_header->name.size++;
                } else {
                        // TODO: 500 Internal Server Error
                        error("header name too long");
                        state->step = PARSE_ERROR;
                }
                state->p++;
        }

        // Skip colon and whitespace
        while (*state->p && (*state->p == ':' || isspace(*state->p)))
                state->p++;

        state->step = PARSE_HEADER_VALUE;
}

void
parse_http_header_value(struct ParseState* state)
{
        // current_header->value has been zero initialized by
        // `da_append` macro

        auto current_header =
            &state->req->headers.items[state->req->headers.len - 1];

        while (*state->p && *state->p != '\r' && *state->p != '\n') {
                char* current_value =
                    arena_alloc_array(arena, 1, current_value);
                if (current_header->value.data == nullptr) {
                        current_header->value.data = current_value;
                }
                if (current_value != nullptr) {
                        *current_value = *state->p;
                        current_header->value.size++;
                } else {
                        // TODO: 500 Internal Server Error
                        error("header value is too long");
                        state->step = PARSE_ERROR;
                }
                state->p++;
        }

        // Skip CRLF
        if (*state->p == '\r') state->p++;
        if (*state->p == '\n') state->p++;

        state->step = PARSE_HEADER_NAME;
}

void
parse_http_body(struct ParseState* state)
{
        size_t bytes_remaining = strlen(state->p);
        assert(state->content_length >= 0);
        size_t bytes_to_copy = ((size_t)state->content_length < bytes_remaining)
                                   ? (size_t)state->content_length
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
                state->step = PARSE_ERROR;
        }

        state->req->body = (struct String){
            .data = body.data,
            .size = bytes_to_copy,
        };
        memcpy(state->req->body.data, state->p, bytes_to_copy);
        state->p += bytes_to_copy;

        state->step = PARSE_COMPLETE;

        debug("Finished parsing body: '%.*s'", STRING_PRINT(state->req->body));
}
