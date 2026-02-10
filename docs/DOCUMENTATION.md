# C HTTP Server Documentation (For Beginners)

*Version: 1.0.0*

*See [VERSION.md](VERSION.md) for documentation changelog*

## Overview

This project is a simple HTTP server written in C. It allows you to create a web server that can handle basic requests from web browsers or other clients.

## What is an HTTP Server?

An HTTP server is a program that:
1. Waits for incoming connections from clients (like web browsers)
2. Receives HTTP requests (like "GET me this webpage")
3. Processes those requests 
4. Sends back HTTP responses (like HTML content)

## Project Structure

The project is organized into these main parts:

```
src/
├── main.c                    # Main program entry point
├── lib/                      # Library code
    ├── http/                 # HTTP protocol handling
    ├── net/                  # Network (socket) handling
    └── env/                  # Environment variable handling
```

## Key Components

### 1. main.c

This is where the program starts. It:
- Sets up the server configuration (port, host address)
- Defines routes (which function handles which URL path)
- Starts the server

```c
// Example from main.c
http.HandleFunc("/", Index);        // When someone visits /, run the Index function
http.HandleFunc("/about", About);   // When someone visits /about, run the About function
http.ListenAndServe(hostname, router); // Start the server
```

### 2. HTTP Module (lib/http/)

This module handles HTTP-specific tasks:
- Parsing HTTP requests (breaking down the data the browser sends)
- Creating HTTP responses (building the data to send back)
- Routing requests to the right function based on URL

#### Key Files:
- **header.h**: Contains definitions for HTTP structures and functions
- **handle.c**: Handles processing of HTTP requests
- **req_parser.c**: Parses raw HTTP request data into structured form

### 3. Network Module (lib/net/)

This module manages the networking aspects:
- Creating a socket (a communication endpoint)
- Binding to a port (setting up where to listen)
- Accepting client connections
- Sending/receiving data

#### Key Files:
- **header.h**: Defines network structures and functions
- **listener.c**: Creates the socket and listens for connections
- **serve.c**: Accepts connections and processes them

### 4. Environment Module (lib/env/)

This simple module lets the program read environment variables, which are settings stored outside the program:
- Getting PORT from environment (to change which port the server listens on)
- Getting HOST from environment (to change which address to bind to)

## How It All Works Together

1. **Startup**: The program starts in main.c, configures settings, and defines routes
2. **Listening**: The server creates a socket and starts listening for connections
3. **Accepting**: When a client connects, the server accepts the connection
4. **Receiving**: The server reads the HTTP request from the client
5. **Parsing**: The request is broken down into its components (method, path, headers, etc.)
6. **Routing**: The server finds which function should handle the request based on the URL path
7. **Processing**: The handler function processes the request
8. **Responding**: The server sends back an HTTP response
9. **Closing**: The connection is closed

## HTTP Request/Response Basics

HTTP communication follows a request-response pattern:

### Request (from client to server)
```
GET /about HTTP/1.1
Host: example.com
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) ...
```

This means:
- Method: GET (asking for data)
- Path: /about (what resource we want)
- Version: HTTP/1.1
- Plus some headers with extra information

### Response (from server to client)
```
HTTP/1.1 200 OK
Content-Type: text/html

<html>
  <body>
    <h1>About Page</h1>
  </body>
</html>
```

This means:
- Version: HTTP/1.1
- Status: 200 OK (success)
- Headers: Content-Type tells the browser this is HTML
- Body: The actual HTML content

## Key C Concepts Used

### 1. Structs
Used to organize related data together, like request information:

```c
struct Request {
    char method[MAX_METHOD_LEN];
    char path[MAX_PATH_LEN];
    char version[MAX_VERSION_LEN];
    struct Header headers[MAX_HEADERS];
    int header_count;
    char body[MAX_BODY_LEN];
    // ...
};
```

### 2. Function Pointers
Used to call different functions based on the requested URL:

```c
typedef int(*HandlerFunc)(ResponseWriter w, Request r);

struct Router {
    char* patterns[50];
    HandlerFunc handlers[50];
};
```

### 3. Sockets
Used for network communication:

```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
```

### 4. Memory Management
For working with dynamic memory:

```c
Request* req = parse_http_request(buffer);
```

## Example: Adding a New Route

If you want to add a new page to your server, you would:

1. Create a handler function:
```c
int Contact(ResponseWriter w, Request r) {
    w.Write("Contact Us Page");
    return 0;
}
```

2. Register the route in main():
```c
http.HandleFunc("/contact", Contact);
```

3. Update your router:
```c
Router router = {{"/","/about","/contact",NULL}, {Index,About,Contact,NULL}};
```

## Glossary

- **HTTP**: HyperText Transfer Protocol - the protocol used for web communication
- **Socket**: A software endpoint that establishes communication between processes
- **Port**: A virtual "doorway" where network connections start and end
- **Request**: A message from client to server asking for something
- **Response**: A message from server to client providing an answer
- **Route/Path**: The part of a URL that specifies which resource is being requested
- **Handler Function**: Code that processes a specific type of request
- **Socket**: A communication endpoint for sending/receiving data over a network
- **Binding**: Associating a socket with an IP address and port number
- **Listening**: Waiting for incoming connection requests
- **Parsing**: Breaking down raw data into a structured format

## Compiling and Running

To compile and run the server:

```bash
make        # Compiles the code and runs the server
```

If the default port (9000) is already in use, you can change it:

```bash
export PORT=8080
make
```

Then, open a web browser and navigate to:
```
http://localhost:8080/
```

## Understanding What Happens When You Run the Server

1. The Makefile compiles all the source files into object files
2. These are linked together into the final executable (bin/server)
3. When run, the server:
   - Configures itself based on environment variables
   - Defines the routes and handlers
   - Creates a socket
   - Binds to the specified port
   - Starts listening for connections
   - When a connection arrives, it accepts it and processes the request
   - It sends back a response
   - Then it waits for the next connection
