# Layer Separation: Network, Application Protocol, and Application Handlers

### 1. Network Layer (`src/lib/net/`)

**Responsibility:** I/O operations and connection lifecycle

**What it does:**
- Accept connections
- Read bytes from socket
- Write bytes to socket  
- Close connections
- Manage socket resources

**What it doesn't know:**
- HTTP protocol details
- Request/response format
- Routing logic

**Key files:**
- `net_serve.c` - Main server loop
- `header.h` - Defines `ProtocolHandler` interface

### 2. HTTP Protocol Layer (`src/lib/http/`)

**Responsibility:** Transform request bytes into response bytes

**What it does:**
- Parse HTTP requests
- Route to handlers
- Build HTTP responses
- Return formatted response bytes

**What it doesn't know:**
- Sockets or file descriptors
- How to read/write data
- Connection state

**Key files:**
- `http_handle.c` - Pure HTTP processing
- `http_handle_connection.c` - Adapter to network layer

### 3. Application Layer (your handlers)

**Responsibility:** Business logic

**What it does:**
- Process requests
- Generate content
- Set status codes and headers

**What it doesn't know:**
- HTTP protocol details
- Network operations
- How responses are sent


## Why This Matters

### Testing

Test HTTP without sockets

```c
const char* request = "GET / HTTP/1.1\r\n\r\n";
HttpResponse resp = http_handle(router, request);
assert(strstr(resp.data, "200 OK"));
```

### Flexibility

Same protocol, different transports


```c
net_serve("localhost:9080", http_handle_connection, ctx);  // TCP
net_serve_ssl("localhost:9443", http_handle_connection, ctx);  // SSL
```

### Composability

For example, wrap protocol layer with middleware

```c
ProtocolResponse logged_handler(RequestContext* ctx, const char* data, size_t len) {
    log_request(data);
    ProtocolResponse resp = http_handle_connection(ctx, data, len);
    log_response(resp.data);
    return resp;
}
```
