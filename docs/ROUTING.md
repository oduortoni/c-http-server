# Routing System Guide

## Overview

The routing system maps URL paths to handler functions using regex patterns. It supports dynamic path segments, sub-routers, and automatic 404 handling.

---

## Quick Start

### 1. Create a Router

```c
Router* router = router_create_regex();
```

### 2. Register Routes

```c
router_add(router, "^/$", Index);
router_add(router, "^/about$", About);
router_add(router, "^/users/([0-9]+)$", UserProfile);
```

### 3. Start the Server

```c
http.ListenAndServe("127.0.0.1:9000", router);
```

---

## Handler Functions

Handlers process requests and write responses:

```c
int Index(ResponseWriter* w, Request* r)
{
    SetStatus(w, 200, "OK");
    SetHeader(w, "Content-Type", "text/html");
    w->WriteString(w, "<h1>Welcome!</h1>");
    return EXIT_SUCCESS;
}
```

**Handler Signature:**
- `ResponseWriter* w` - Write response data
- `Request* r` - Read request data (method, path, headers, body)
- Returns `int` - `EXIT_SUCCESS` or error code

---

## Route Patterns

Routes use POSIX Extended Regular Expressions:

### Static Routes

```c
router_add(router, "^/$", Index);           // Exact match: /
router_add(router, "^/about$", About);      // Exact match: /about
router_add(router, "^/contact$", Contact);  // Exact match: /contact
```

**Important:** Always use `^` (start) and `$` (end) anchors for exact matching.

### Dynamic Routes (Capture Groups)

```c
// Match: /users/123, /users/456
router_add(router, "^/users/([0-9]+)$", UserProfile);

// Match: /posts/hello-world, /posts/my-post
router_add(router, "^/posts/([a-z-]+)$", BlogPost);

// Match: /static/style.css, /static/images/logo.png
router_add(router, "^/static/(.*)$", StaticFiles);
```

### Accessing Captured Values

Use `regmatch_t` to extract captured segments:

```c
int UserProfile(ResponseWriter* w, Request* r)
{
    // Extract user ID from path
    regmatch_t match = r->path_matches[1];  // First capture group
    int start = match.rm_so;
    int end = match.rm_eo;
    
    char user_id[32];
    snprintf(user_id, sizeof(user_id), "%.*s", end - start, r->path.data + start);
    
    SetStatus(w, 200, "OK");
    w->WriteString(w, "User ID: ");
    w->WriteString(w, user_id);
    
    return EXIT_SUCCESS;
}
```

**Note:** `path_matches[0]` is the full match, `path_matches[1]` is the first capture group, etc.

---

## Request Object

Access request data through the `Request` struct:

```c
int Handler(ResponseWriter* w, Request* r)
{
    // HTTP method
    if (strcmp(r->method, "POST") == 0) {
        // Handle POST request
    }
    
    // Request path
    printf("Path: %.*s\n", (int)r->path.size, r->path.data);
    
    // Request body
    if (r->body.size > 0) {
        printf("Body: %.*s\n", (int)r->body.size, r->body.data);
    }
    
    // Headers (iterate through headers array)
    for (size_t i = 0; i < r->headers.len; i++) {
        printf("Header: %.*s = %.*s\n",
               (int)r->headers.items[i].name.size,
               r->headers.items[i].name.data,
               (int)r->headers.items[i].value.size,
               r->headers.items[i].value.data);
    }
    
    return EXIT_SUCCESS;
}
```

---

## Response Writer

Build responses using the `ResponseWriter` object:

### Set Status

```c
SetStatus(w, 200, "OK");
SetStatus(w, 404, "Not Found");
SetStatus(w, 500, "Internal Server Error");
```

### Set Headers

```c
SetHeader(w, "Content-Type", "text/html");
SetHeader(w, "Content-Type", "application/json");
SetHeader(w, "Cache-Control", "no-cache");
```

### Write Body

```c
// Write string
w->WriteString(w, "<h1>Hello World</h1>");

// Write formatted data
char buffer[256];
snprintf(buffer, sizeof(buffer), "User ID: %d", user_id);
w->WriteString(w, buffer);

// Write raw data
response_write(w, data, data_length);
```

---

## Form Data

Parse form submissions:

```c
int ContactForm(ResponseWriter* w, Request* r)
{
    if (strcmp(r->method, "POST") == 0) {
        FormData form_data = {0};
        parse_form_data(r->body, &form_data);
        
        const char* name = get_form_value(&form_data, "name");
        const char* email = get_form_value(&form_data, "email");
        const char* message = get_form_value(&form_data, "message");
        
        if (!name || !email || !message) {
            SetStatus(w, 400, "Bad Request");
            w->WriteString(w, "Missing required fields");
            return -1;
        }
        
        // Process form data...
        SetStatus(w, 200, "OK");
        w->WriteString(w, "Form submitted successfully!");
        return EXIT_SUCCESS;
    }
    
    // Show form for GET requests
    SetStatus(w, 200, "OK");
    SetHeader(w, "Content-Type", "text/html");
    w->WriteString(w, "<form method='POST'>...</form>");
    return EXIT_SUCCESS;
}
```

---

## Sub-Routers

Organize routes into modules using sub-routers:

### Create a Sub-Router

**`src/app/blog/router.c`**
```c
#include "header.h"
#include "http/header.h"

Router* BlogRouter()
{
    Router* router = router_create_regex();
    router_add(router, "^/blog$", BlogIndex);
    router_add(router, "^/blog/([0-9]+)$", BlogPost);
    router_add(router, "^/blog/new$", BlogNew);
    return router;
}
```

### Mount Sub-Router

**`src/main.c`**
```c
Router* router = router_create_regex();
router_add(router, "^/$", Index);

// Mount blog router at /apps prefix
// Routes become: /apps/blog, /apps/blog/123, /apps/blog/new
router_mount(router, "/apps", BlogRouter());

http.ListenAndServe("127.0.0.1:9000", router);
```

**How it works:**
- Child pattern `^/blog$` + prefix `/apps` → `^/apps/blog$`
- Child pattern `^/blog/([0-9]+)$` + prefix `/apps` → `^/apps/blog/([0-9]+)$`

---

## 404 Handling

### Custom 404 Handler

```c
int Error404(ResponseWriter* w, Request* r)
{
    SetStatus(w, 404, "Not Found");
    SetHeader(w, "Content-Type", "text/html");
    w->WriteString(w, "<h1>404 - Page Not Found</h1>");
    return EXIT_SUCCESS;
}

// Register 404 handler
router_add(router, "^/404$", Error404);
```

The router automatically uses the `^/404$` route when no other route matches.

### Default 404

If no `^/404$` route is registered, the server returns a plain text 404 response.

---

## Complete Example

**`src/main.c`**
```c
#include "app/header.h"
#include "http/header.h"

int main()
{
    // Create main router
    Router* router = router_create_regex();
    
    // Static routes
    router_add(router, "^/$", Index);
    router_add(router, "^/about$", About);
    router_add(router, "^/contact$", Contact);
    
    // Dynamic routes
    router_add(router, "^/users/([0-9]+)$", UserProfile);
    router_add(router, "^/posts/([a-z0-9-]+)$", BlogPost);
    
    // Static files
    router_add(router, "^/static/(.*)$", StaticFiles);
    
    // 404 handler
    router_add(router, "^/404$", Error404);
    
    // Mount sub-routers
    router_mount(router, "/api", ApiRouter());
    router_mount(router, "/admin", AdminRouter());
    
    // Start server
    printf("Server listening on http://127.0.0.1:9000\n");
    http.ListenAndServe("127.0.0.1:9000", router);
    
    // Cleanup (never reached due to infinite loop)
    router_free(router);
    
    return 0;
}
```

**`src/app/handlers.c`**
```c
#include "header.h"

int Index(ResponseWriter* w, Request* r)
{
    SetStatus(w, 200, "OK");
    SetHeader(w, "Content-Type", "text/html");
    w->WriteString(w, "<h1>Welcome to C HTTP Server</h1>");
    return EXIT_SUCCESS;
}

int UserProfile(ResponseWriter* w, Request* r)
{
    // Extract user ID from URL
    regmatch_t match = r->path_matches[1];
    char user_id[32];
    snprintf(user_id, sizeof(user_id), "%.*s",
             match.rm_eo - match.rm_so,
             r->path.data + match.rm_so);
    
    SetStatus(w, 200, "OK");
    SetHeader(w, "Content-Type", "text/html");
    
    char response[256];
    snprintf(response, sizeof(response),
             "<h1>User Profile</h1><p>User ID: %s</p>", user_id);
    w->WriteString(w, response);
    
    return EXIT_SUCCESS;
}

int Error404(ResponseWriter* w, Request* r)
{
    SetStatus(w, 404, "Not Found");
    SetHeader(w, "Content-Type", "text/html");
    w->WriteString(w, 
        "<html><body>"
        "<h1>404 - Page Not Found</h1>"
        "<p>The requested page does not exist.</p>"
        "</body></html>");
    return EXIT_SUCCESS;
}
```

---

## Common Patterns

### REST API Routes

```c
router_add(router, "^/api/users$", UsersIndex);           // GET /api/users
router_add(router, "^/api/users/([0-9]+)$", UsersShow);   // GET /api/users/123
router_add(router, "^/api/posts$", PostsIndex);           // GET /api/posts
router_add(router, "^/api/posts/([0-9]+)$", PostsShow);   // GET /api/posts/456
```

Handle different HTTP methods in the handler:

```c
int UsersIndex(ResponseWriter* w, Request* r)
{
    if (strcmp(r->method, "GET") == 0) {
        // List users
    } else if (strcmp(r->method, "POST") == 0) {
        // Create user
    } else {
        SetStatus(w, 405, "Method Not Allowed");
        return -1;
    }
    return EXIT_SUCCESS;
}
```

### File Extensions

```c
// Match: /page.html, /about.html
router_add(router, "^/([a-z]+)\\.html$", HtmlPages);

// Match: /api/v1/users, /api/v2/users
router_add(router, "^/api/(v[0-9]+)/users$", ApiUsers);
```

### Optional Segments

```c
// Match: /search or /search/query
router_add(router, "^/search(/.*)?$", Search);
```

---

## Best Practices

1. **Always use anchors:** Start patterns with `^` and end with `$`
2. **Order matters:** More specific routes should come before general ones
3. **Validate input:** Always check captured values before using them
4. **Return proper status codes:** Use appropriate HTTP status codes
5. **Set Content-Type:** Always set the correct content type header
6. **Handle errors:** Return error codes when operations fail
7. **Free resources:** Clean up allocated memory in handlers
8. **Use sub-routers:** Organize related routes into modules

---

## Regex Quick Reference

| Pattern | Matches | Example |
|---------|---------|---------|
| `^/path$` | Exact path | `/path` |
| `[0-9]+` | One or more digits | `123`, `456` |
| `[a-z]+` | One or more lowercase letters | `hello`, `world` |
| `[a-zA-Z0-9-]+` | Alphanumeric with hyphens | `my-post-123` |
| `.*` | Any characters (greedy) | `anything/here` |
| `[^/]+` | Any characters except `/` | `filename` |
| `(/.*)?` | Optional path segment | `/optional` or empty |
| `\\.` | Literal dot | `.html` |

---

## Troubleshooting

### Route Not Matching

- Check that pattern has `^` and `$` anchors
- Test regex pattern with online tools
- Verify route is registered before starting server
- Check route order (specific before general)

### Capture Groups Not Working

- Ensure pattern has parentheses: `([0-9]+)`
- Use correct index: `path_matches[1]` for first group
- Check `rm_so` and `rm_eo` are valid before accessing

### 404 Always Returned

- Verify handler function signature is correct
- Check that route pattern matches the URL exactly
- Ensure router is passed to `ListenAndServe`

---

## Memory Management

The router automatically manages memory for routes. Always free the router when done:

```c
router_free(router);
```

**Note:** In the current implementation, the server runs indefinitely, so `router_free` is never reached. For testing or cleanup, you can call it explicitly.

---

## See Also

- [Server Architecture](SERVER_ARCHITECTURE.md) - Overall system design
- [Examples](EXAMPLES.md) - More code examples
- [Template System](TEMPLATE_SYSTEM.md) - Rendering HTML responses
