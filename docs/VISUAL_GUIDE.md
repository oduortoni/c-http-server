# C HTTP Server: Visual Guide

*Version: 1.0.0*

*See [VERSION.md](VERSION.md) for documentation changelog*

This guide uses simple ASCII diagrams to explain how the C HTTP server works.

## 1. The Big Picture

```
                                    +------------+
                                    |            |
+--------+    HTTP Request          |  C HTTP    |
| Browser | ----------------------> |  Server    |
|        | <----------------------- |            |
+--------+    HTTP Response         +------------+
```

## 2. Server Initialization

```
+----------------+      +-----------------+      +----------------+
| Read           |      | Set up routes   |      | Create socket  |
| environment    | ---> | and register    | ---> | and bind to    |
| variables      |      | handler funcs   |      | port           |
+----------------+      +-----------------+      +----------------+
                                                        |
                                                        v
                                                +----------------+
                                                | Start listening|
                                                | for incoming   |
                                                | connections    |
                                                +----------------+
```

## 3. Processing a Request

```
+----------------+      +-----------------+      +----------------+
| Accept         |      | Read data from  |      | Parse HTTP     |
| client         | ---> | client socket   | ---> | request into   |
| connection     |      |                 |      | struct         |
+----------------+      +-----------------+      +----------------+
        |                                                |
        v                                                v
+----------------+      +-----------------+      +----------------+
| Close          |      | Send HTTP       |      | Find matching  |
| connection     | <--- | response to     | <--- | route handler  |
|                |      | client          |      | function       |
+----------------+      +-----------------+      +----------------+
                                                        |
                                                        v
                                                +----------------+
                                                | Execute        |
                                                | handler        |
                                                | function       |
                                                +----------------+
```

## 4. HTTP Request Structure

```
+-------------------------------------------------+
| GET /about HTTP/1.1                             | <- Request Line (Method, Path, Version)
+-------------------------------------------------+
| Host: example.com                               | 
| User-Agent: Mozilla/5.0                         | <- Headers
| Accept: text/html                               |
+-------------------------------------------------+
|                                                 | <- Empty line separates headers from body
+-------------------------------------------------+
| (request body - empty for GET requests)         | <- Body (optional)
+-------------------------------------------------+
```

## 5. HTTP Response Structure

```
+-------------------------------------------------+
| HTTP/1.1 200 OK                                 | <- Status Line
+-------------------------------------------------+
| Content-Type: text/html                         | 
| Content-Length: 123                             | <- Headers
| Server: C-HTTP-Server                           |
+-------------------------------------------------+
|                                                 | <- Empty line separates headers from body
+-------------------------------------------------+
| <!DOCTYPE html>                                 |
| <html>                                          |
|   <body>                                        | <- Body
|     <h1>About Page</h1>                         |
|   </body>                                       |
| </html>                                         |
+-------------------------------------------------+
```

## 6. Code Structure

```
src/
|
|-- main.c (Entry point)
|
|-- lib/
    |
    |-- http/ (HTTP Protocol handling)
    |   |-- header.h (Definitions)
    |   |-- handle.c (Request processing)
    |   |-- req_parser.c (Request parsing)
    |   |-- ...
    |
    |-- net/ (Network operations)
    |   |-- header.h (Definitions)
    |   |-- listener.c (Socket creation)
    |   |-- serve.c (Connection handling)
    |   |-- ...
    |
    |-- env/ (Environment variables)
        |-- header.h (Definitions)
        |-- get_env_variable.c (Env var access)
```

## 7. Function Call Flow

```
                                +---------------+
                                | main()        |
                                +---------------+
                                        |
                       +----------------+--------------+
                       |                               |
               +---------------+                +---------------+
               | http.HandleFunc() |            | http.ListenAndServe() |
               +---------------+                +---------------+
                                                        |
                                                +---------------+
                                                | serve()       |
                                                +---------------+
                                                        |
                                                +---------------+
                                                | listener()    | Creates socket
                                                +---------------+
                                                        |
                                          Loop +---------------+
                                        +------+ accept()      | Accepts connection
                                        |      +---------------+
                                        |               |
                                        |      +---------------+
                                        |      | handle()      | Process request
                                        |      +---------------+
                                        |               |
                                        |      +---------------+
                                        |      | parse_http_request() |
                                        |      +---------------+
                                        |               |
                                        |      +---------------+
                                        +------+ write()       | Send response
                                               +---------------+
```

## 8. Memory Layout of Key Structures

### Request Structure

```
+---------------------------------------------+
| Request                                     |
+---------------------------------------------+
| char method[8]       | "GET", "POST", etc.  |
| char path[256]       | "/about", "/"        |
| char version[16]     | "HTTP/1.1"           |
| Header headers[20]   | Array of headers     |
| int header_count     | Number of headers    |
| char body[4096]      | Request body data    |
| size_t body_length   | Length of body       |
| function pointers... | For I/O operations   |
+---------------------------------------------+
```

### Router Structure

```
+---------------------------------------------+
| Router                                      |
+---------------------------------------------+
| char* patterns[50]   | URLs: "/", "/about"  |
| HandlerFunc handlers[50] | Function pointers|
+---------------------------------------------+
```

This visual guide should help you better understand how the different parts of the C HTTP server fit together and work to process web requests. 