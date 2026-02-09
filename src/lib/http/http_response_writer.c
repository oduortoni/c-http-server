#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "header.h"
#include "utils/arena/header.h"
#include "utils/logging/header.h"

int
response_write(ResponseWriter* rw, const char* data, size_t size)
{
        size_t remaining = MAX_BODY_LEN - rw->body_length;
        size_t to_write  = size < remaining ? size : remaining;

        if (to_write > 0) {
                memcpy(rw->body + rw->body_length, data, to_write);
                rw->body_length += to_write;
        }
        return to_write;
}

int
response_write_string(ResponseWriter* rw, const char* str)
{
        return rw->Write(rw, str, strlen(str));
}

// Initialization function
void
InitResponseWriter(ResponseWriter* rw)
{
        static thread_local struct Arena arena = {0};

        if (arena.buf == nullptr) {
                constexpr size_t buf_len = 1024 * 1024;
                char* buf                = malloc(buf_len);
                if (!buf) {
                        // TODO: handle allocation error
                        error("Coun't allocate memory for arena's buffer");
                        return;
                }
                arena_init(&arena, buf, buf_len);
        }

        memset(rw, 0, sizeof *rw);
        strncpy(rw->version, "HTTP/1.1", MAX_VERSION_LEN);
        rw->allocator   = &arena;
        rw->Write       = response_write;
        rw->WriteString = response_write_string;
}

// Header management functions
void
SetHeader(ResponseWriter* rw, const char* name, const char* value)
{
        size_t const name_len  = strlen(name);
        size_t const value_len = strlen(value);
        for (int i = 0; i < rw->header_count; ++i) {
                Header* h = &rw->headers[i];
                if (name_len == h->name.size &&
                    memcmp(h->name.data, name, h->name.size) == 0) {
                        h->value.size = value_len;
                        h->value.data =
                            arena_alloc(rw->allocator, h->value.size);
                        strncpy(h->value.data, value, h->value.size);
                        return;
                }
        }
        if (rw->header_count < MAX_HEADERS) {
                Header* h    = &rw->headers[rw->header_count++];

                h->name.size = strlen(name);
                h->name.data = arena_alloc(rw->allocator, h->name.size);
                strncpy(h->name.data, name, h->name.size);

                h->value.size = strlen(value);
                h->value.data = arena_alloc(rw->allocator, h->value.size);
                strncpy(h->value.data, value, h->value.size);
        }
}

// Status code setting
void
SetStatus(ResponseWriter* rw, int code, const char* text)
{
        rw->status_code = code;
        strncpy(rw->status_text, text, MAX_STATUS_LEN - 1);
}

// Response construction
char*
BuildResponse(ResponseWriter* rw)
{
        char* response =
            malloc(4096);  // we can avoid using the constant 4096 later
        if (!response) return nullptr;

        constexpr size_t response_size = 4096 - 1;
        char* ptr                      = response;

        // Status line
        int written = snprintf(ptr, response_size, "%s %d %s\r\n", rw->version,
                               rw->status_code, rw->status_text);
        ptr += written;

        // Headers
        for (int i = 0; i < rw->header_count; i++) {
                written = snprintf(ptr, response + response_size - ptr,
                                   "%.*s: %.*s\r\n",
                                   STRING_PRINT(rw->headers[i].name),
                                   STRING_PRINT(rw->headers[i].value));
                ptr += written;
        }

        // End of headers
        strncpy(ptr, "\r\n", response + response_size - ptr);
        ptr += 2;

        // Body
        if (rw->body_length > 0) {
                assert(rw->body_length < INT64_MAX);
                size_t to_copy =
                    (long)rw->body_length < (response + response_size - ptr)
                        ? (long)rw->body_length
                        : (response + response_size - ptr);
                memcpy(ptr, rw->body, to_copy);
                ptr += to_copy;
        }

        return response;
}
