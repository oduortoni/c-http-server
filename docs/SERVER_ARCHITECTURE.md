# Server Architecture

## Overview

The C HTTP Server follows a layered architecture that separates network concerns from protocol-specific logic. This design ensures protocol independence and prevents circular dependencies.

## Architecture Layers

```
┌─────────────────────────────────────────┐
│           Application Layer             │
│        (Route Handlers: /, /about)      │
└─────────────────────────────────────────┘
                    │
┌─────────────────────────────────────────┐
│           HTTP Protocol Layer           │
│     (http_handle, http_handle_connection)│
│        - Request parsing                │
│        - Response building              │
│        - Routing logic                  │
└─────────────────────────────────────────┘
                    │ provides handler
┌─────────────────────────────────────────┐
│           Network Layer (TCP)           │
│      (net_serve, net_listener)          │
│        - Socket management             │
│        - Connection acceptance          │
│        - Generic callback system       │
└─────────────────────────────────────────┘
```

## Dependency Direction (Critical Design Decision)

The architecture follows a **unidirectional dependency** pattern:

```
HTTP Layer ──depends on──> NET Layer
NET Layer  ──independent──> (no dependencies)
```

### Why This Matters

1. **NET Layer Independence**: The network layer knows nothing about HTTP or any specific protocol
2. **Protocol Responsibility**: HTTP layer provides its own handler to the NET layer
3. **No Circular Dependencies**: Clean, maintainable dependency graph

## Network Layer (Generic TCP Server)

The `net` package provides a protocol-agnostic TCP server:

```c
// NET layer signature - accepts ANY protocol handler
int net_serve(char *host, ProtocolHandler handler, RequestContext *context);
```

### Key Design Principles

- **Protocol Agnostic**: No knowledge of HTTP, FTP, or any specific protocol
- **Callback-Based**: Accepts handler functions from protocol layers
- **Context Passing**: Generic context mechanism for protocol data
- **Pure TCP Concerns**: Only handles sockets, connections, and callbacks

## HTTP Protocol Layer Integration

The HTTP layer **provides** its handler to the NET layer:

```c
// HTTP layer calls NET layer with its own handler
net_serve(host, http_handle_connection, &context);
```

### Why HTTP Provides the Handler

1. **Separation of Concerns**: NET doesn't need to know about HTTP specifics
2. **Extensibility**: Other protocols can provide their own handlers
3. **Testability**: Each layer can be tested independently
4. **Maintainability**: Changes in HTTP don't affect NET layer

## Protocol Handler Pattern

```c
// Generic protocol handler signature
typedef int (*ProtocolHandler)(RequestContext *context, Client client);

// HTTP implements this interface
int http_handle_connection(RequestContext *context, Client client);

// Future protocols can implement the same interface
int ftp_handle_connection(RequestContext *context, Client client);
int smtp_handle_connection(RequestContext *context, Client client);
```

## Benefits of This Architecture

### 1. Protocol Independence
```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│    HTTP     │    │    FTP      │    │   Custom    │
│   Server    │    │   Server    │    │  Protocol   │
└─────────────┘    └─────────────┘    └─────────────┘
       │                   │                   │
       │ provides handler  │ provides handler  │ provides handler
       └───────────────────┼───────────────────┘
                           ▼
                ┌─────────────────┐
                │   NET Server    │
                │  (TCP Layer)    │ ◄─── No protocol knowledge
                └─────────────────┘
```

### 2. Clean Dependency Graph
- **NET Layer**: Zero dependencies on protocol layers
- **HTTP Layer**: Depends only on NET layer
- **Application Layer**: Depends only on HTTP layer

### 3. Independent Evolution
- Network optimizations don't affect protocol logic
- Protocol changes don't require network layer modifications
- New protocols can be added without touching existing code

## Flow Diagram

```
Client Request
      │
      ▼
┌─────────────┐
│ net_serve() │ ◄─── Accepts TCP connection
└─────────────┘
      │
      ▼ calls provided handler
┌─────────────────────┐
│http_handle_connection│ ◄─── HTTP-specific processing
└─────────────────────┘
      │
      ▼
┌─────────────┐
│http_handle()│ ◄─── Parse HTTP, route request
└─────────────┘
      │
      ▼
┌─────────────┐
│Route Handler│ ◄─── Execute application logic
└─────────────┘
```

## Implementation Example

### NET Layer (Protocol Agnostic)
```c
int net_serve(char *host, ProtocolHandler handler, RequestContext *context) {
    // ... TCP socket setup ...
    while(1) {
        Client client = accept_connection();
        handler(context, client);  // Calls protocol-specific handler
    }
}
```

### HTTP Layer (Protocol Specific)
```c
int listenAndServe(char *host, Router *router) {
    RequestContext context = {.router = router};
    // HTTP provides its handler to NET
    net_serve(host, http_handle_connection, &context);
}
```

## Why This Architecture Prevents Common Problems

1. **Circular Dependencies**: NET never imports HTTP headers
2. **Tight Coupling**: Each layer has a single, well-defined responsibility
3. **Testing Complexity**: Each layer can be unit tested in isolation
4. **Code Reuse**: NET layer can be reused by any TCP-based protocol

This architecture ensures that the TCP server remains a pure, reusable component while allowing protocols to implement their specific requirements independently.