#include "header.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int response_write(ResponseWriter* rw, const char* data, size_t size) {
    size_t remaining = MAX_BODY_LEN - rw->body_length;
    size_t to_write = size < remaining ? size : remaining;
    
    if(to_write > 0) {
        memcpy(rw->body + rw->body_length, data, to_write);
        rw->body_length += to_write;
    }
    return to_write;
}

int response_write_string(ResponseWriter* rw, const char* str) {
    return rw->Write(rw, str, strlen(str));
}

// Initialization function
void InitResponseWriter(ResponseWriter* rw) {
    memset(rw, 0, sizeof(ResponseWriter));
    strncpy(rw->version, "HTTP/1.1", MAX_VERSION_LEN);
    rw->Write = response_write;
    rw->WriteString = response_write_string;
}

// Header management functions
void SetHeader(ResponseWriter* rw, const char* name, const char* value) {
    for (int i = 0; i < rw->header_count; ++i) {
        if (strcmp(rw->headers[i].name, name) == 0) {
            Header* h = &rw->headers[i];
            strncpy(h->value, value, sizeof(h->value));
            return;
        }
    }
    if(rw->header_count < MAX_HEADERS) {
        Header* h = &rw->headers[rw->header_count++];
        strncpy(h->name, name, sizeof(h->name));
        strncpy(h->value, value, sizeof(h->value));
    }
}

// Status code setting
void SetStatus(ResponseWriter* rw, int code, const char* text) {
    rw->status_code = code;
    strncpy(rw->status_text, text, MAX_STATUS_LEN);
}

// Response construction
char* BuildResponse(ResponseWriter* rw) {
    static char response[4096]; // should be dynamically allocated
    char* ptr = response;
    
    // Status line
    int written = snprintf(ptr, sizeof(response), "%s %d %s\r\n",
                          rw->version, rw->status_code, rw->status_text);
    ptr += written;
    
    // Headers
    for(int i = 0; i < rw->header_count; i++) {
        written = snprintf(ptr, response + sizeof(response) - ptr, 
                          "%s: %s\r\n", rw->headers[i].name, rw->headers[i].value);
        ptr += written;
    }
    
    // End of headers
    strncpy(ptr, "\r\n", response + sizeof(response) - ptr);
    ptr += 2;
    
    // Body
    if(rw->body_length > 0) {
        assert(rw->body_length < INT64_MAX);
        size_t to_copy = (long)rw->body_length < (response + sizeof(response) - ptr)
            ? (long)rw->body_length : (response + sizeof(response) - ptr);
        memcpy(ptr, rw->body, to_copy);
        ptr += to_copy;
    }
    
    return response;
}
