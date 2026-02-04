#include "header.h"
#include <assert.h>

Request* parse_http_request(const char* raw_request) {
    Request* req = malloc(sizeof *req);
    if (!req) return NULL;
    
    memset(req, 0, sizeof *req);
    req->header_count = 0;
    req->body_length = 0;
    
    enum ParseState state = PARSE_METHOD;
    const char* p = raw_request;
    char* current_name = NULL;
    char* current_value = NULL;
    int content_length = 0;
    
    while (*p && state != PARSE_COMPLETE && state != PARSE_ERROR) {
        switch (state) {
            case PARSE_METHOD: {
                char* method_ptr = req->method;
                while (*p && !isspace(*p)) {
                    if (method_ptr - req->method < MAX_METHOD_LEN - 1) {
                        *method_ptr++ = *p;
                    }
                    p++;
                }
                *method_ptr = '\0';
                if (*p) p++; // skip space
                state = PARSE_PATH;
                break;
            }
            
            case PARSE_PATH: {
                char* path_ptr = req->path;
                while (*p && !isspace(*p)) {
                    if (path_ptr - req->path < MAX_PATH_LEN - 1) {
                        *path_ptr++ = *p;
                    }
                    p++;
                }
                *path_ptr = '\0';
                if (*p) p++; // skip space
                state = PARSE_VERSION;
                break;
            }
            
            case PARSE_VERSION: {
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
                
                state = PARSE_HEADER_NAME;
                break;
            }
            
            case PARSE_HEADER_NAME: {
                if (*p == '\r' || *p == '\n') {
                    if (*p == '\r') p++;
                    if (*p == '\n') p++;
                    
                    // Check for Content-Length
                    for (int i = 0; i < req->header_count; i++) {
                        if (strcasecmp(req->headers[i].name, "Content-Length") == 0) {
                            content_length = atoi(req->headers[i].value);
                        }
                    }
                    
                    state = (content_length > 0 && content_length < MAX_BODY_LEN) ? 
                          PARSE_BODY : PARSE_COMPLETE;
                    break;
                }
                
                if (req->header_count >= MAX_HEADERS) {
                    state = PARSE_ERROR;
                    break;
                }
                
                current_name = req->headers[req->header_count].name;
                while (*p && *p != ':' && !isspace(*p)) {
                    if (current_name - req->headers[req->header_count].name < (MAX_HEADER_LEN/2) - 1) {
                        *current_name++ = *p;
                    }
                    p++;
                }
                *current_name = '\0';
                
                // Skip colon and whitespace
                while (*p && (*p == ':' || isspace(*p))) p++;
                
                current_value = req->headers[req->header_count].value;
                state = PARSE_HEADER_VALUE;
                break;
            }
            
            case PARSE_HEADER_VALUE: {
                while (*p && *p != '\r' && *p != '\n') {
                    if (current_value - req->headers[req->header_count].value < (MAX_HEADER_LEN/2) - 1) {
                        *current_value++ = *p;
                    }
                    p++;
                }
                *current_value = '\0';
                
                // Skip CRLF
                if (*p == '\r') p++;
                if (*p == '\n') p++;
                
                req->header_count++;
                state = PARSE_HEADER_NAME;
                break;
            }
            
            case PARSE_BODY: {
                size_t bytes_remaining = strlen(p);
                assert(content_length >= 0);
                size_t bytes_to_copy = ((size_t)content_length < bytes_remaining)
                    ? (size_t)content_length : bytes_remaining;
                
                if (bytes_to_copy > 0 && bytes_to_copy < MAX_BODY_LEN) {
                    memcpy(req->body, p, bytes_to_copy);
                    req->body_length = bytes_to_copy;
                    req->body[bytes_to_copy] = '\0';
                    p += bytes_to_copy;
                }
                
                state = PARSE_COMPLETE;
                break;
            }
            
            default:
                state = PARSE_ERROR;
                break;
        }
    }
    
    if (state == PARSE_ERROR || req->header_count >= MAX_HEADERS) {
        free(req);
        return NULL;
    }
    
    return req;
}

void free_request(Request* req) {
    free(req);
}
