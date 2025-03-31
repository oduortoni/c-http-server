# C HTTP Server: Practical Examples

*Version: 1.0.0*

*See [VERSION.md](VERSION.md) for documentation changelog*

This document provides practical examples for beginners showing how to extend and customize the C HTTP server with common use cases.

## Example 1: Serving HTML Content

To serve a simple HTML page, modify your handler function like this:

```c
int HomePage(ResponseWriter w, Request r) {
    const char* html = 
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "    <title>My C Web Server</title>"
        "    <style>"
        "        body { font-family: Arial, sans-serif; margin: 40px; line-height: 1.6; }"
        "        h1 { color: #333; }"
        "    </style>"
        "</head>"
        "<body>"
        "    <h1>Welcome to My C Web Server</h1>"
        "    <p>This page is being served by a custom HTTP server written in C.</p>"
        "    <ul>"
        "        <li><a href='/about'>About</a></li>"
        "        <li><a href='/contact'>Contact</a></li>"
        "    </ul>"
        "</body>"
        "</html>";
    
    w.Write(html);
    return 0;
}
```

Then register it in `main.c`:

```c
http.HandleFunc("/", HomePage);
```

## Example 2: Adding a New Route

To add a new page to your server:

```c
int ContactPage(ResponseWriter w, Request r) {
    const char* html = 
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "    <title>Contact Us</title>"
        "    <style>"
        "        body { font-family: Arial, sans-serif; margin: 40px; line-height: 1.6; }"
        "        h1 { color: #333; }"
        "    </style>"
        "</head>"
        "<body>"
        "    <h1>Contact Us</h1>"
        "    <p>Email: example@example.com</p>"
        "    <p>Phone: 123-456-7890</p>"
        "    <p><a href='/'>Back to Home</a></p>"
        "</body>"
        "</html>";
    
    w.Write(html);
    return 0;
}
```

Register it in `main.c`:

```c
http.HandleFunc("/contact", ContactPage);

// Don't forget to update your router:
Router router = {{"/", "/about", "/contact", NULL}, {HomePage, AboutPage, ContactPage, NULL}};
```

## Example 3: Reading GET Parameters

To read parameters from a URL like `/search?query=example`:

```c
int SearchPage(ResponseWriter w, Request r) {
    // Find the query parameter in the path
    char* query = NULL;
    char* question_mark = strchr(r.path, '?');
    
    if (question_mark) {
        // Check if it has "query=" after the question mark
        char* query_param = strstr(question_mark + 1, "query=");
        
        if (query_param) {
            // Extract the query value
            query = query_param + 6; // 6 is the length of "query="
            
            // Find the end of the query (& or end of string)
            char* amp = strchr(query, '&');
            if (amp) {
                *amp = '\0'; // Terminate the string at the &
            }
        }
    }
    
    // Create response with the query
    char response[1024];
    snprintf(response, sizeof(response),
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "    <title>Search Results</title>"
        "    <style>"
        "        body { font-family: Arial, sans-serif; margin: 40px; line-height: 1.6; }"
        "        h1 { color: #333; }"
        "    </style>"
        "</head>"
        "<body>"
        "    <h1>Search Results</h1>"
        "    <p>You searched for: %s</p>"
        "    <form action='/search' method='GET'>"
        "        <input type='text' name='query' placeholder='Search...'>"
        "        <button type='submit'>Search</button>"
        "    </form>"
        "    <p><a href='/'>Back to Home</a></p>"
        "</body>"
        "</html>",
        query ? query : "nothing");
    
    w.Write(response);
    return 0;
}
```

Register it in `main.c`:

```c
http.HandleFunc("/search", SearchPage);
```

## Example 4: Handling POST Requests

To handle form submissions with POST:

```c
int FormHandler(ResponseWriter w, Request r) {
    if (strcmp(r.method, "GET") == 0) {
        // Show the form on GET requests
        const char* form_html =
            "<!DOCTYPE html>"
            "<html>"
            "<head>"
            "    <title>Contact Form</title>"
            "    <style>"
            "        body { font-family: Arial, sans-serif; margin: 40px; line-height: 1.6; }"
            "        h1 { color: #333; }"
            "        .form-group { margin-bottom: 15px; }"
            "        label { display: block; margin-bottom: 5px; }"
            "        input, textarea { width: 100%; padding: 8px; }"
            "        button { padding: 10px 15px; background: #4CAF50; color: white; border: none; }"
            "    </style>"
            "</head>"
            "<body>"
            "    <h1>Contact Form</h1>"
            "    <form method='POST' action='/form'>"
            "        <div class='form-group'>"
            "            <label for='name'>Name:</label>"
            "            <input type='text' id='name' name='name' required>"
            "        </div>"
            "        <div class='form-group'>"
            "            <label for='email'>Email:</label>"
            "            <input type='email' id='email' name='email' required>"
            "        </div>"
            "        <div class='form-group'>"
            "            <label for='message'>Message:</label>"
            "            <textarea id='message' name='message' rows='4' required></textarea>"
            "        </div>"
            "        <button type='submit'>Send Message</button>"
            "    </form>"
            "    <p><a href='/'>Back to Home</a></p>"
            "</body>"
            "</html>";
        
        w.Write(form_html);
    } 
    else if (strcmp(r.method, "POST") == 0) {
        // Handle the form submission on POST requests
        
        // Simple parsing of form data (name=value&name2=value2)
        char name[256] = {0};
        char email[256] = {0};
        char message[1024] = {0};
        
        // Parse the body (very simple parsing, not for production)
        char* body = r.body;
        
        // Find the name field
        char* name_field = strstr(body, "name=");
        if (name_field) {
            name_field += 5; // Skip "name="
            char* end = strchr(name_field, '&');
            if (end) {
                strncpy(name, name_field, end - name_field);
                name[end - name_field] = '\0';
            } else {
                strcpy(name, name_field);
            }
        }
        
        // Find the email field
        char* email_field = strstr(body, "email=");
        if (email_field) {
            email_field += 6; // Skip "email="
            char* end = strchr(email_field, '&');
            if (end) {
                strncpy(email, email_field, end - email_field);
                email[end - email_field] = '\0';
            } else {
                strcpy(email, email_field);
            }
        }
        
        // Find the message field
        char* message_field = strstr(body, "message=");
        if (message_field) {
            message_field += 8; // Skip "message="
            strcpy(message, message_field);
        }
        
        // Create a response with the submitted data
        char response[2048];
        snprintf(response, sizeof(response),
            "<!DOCTYPE html>"
            "<html>"
            "<head>"
            "    <title>Form Submitted</title>"
            "    <style>"
            "        body { font-family: Arial, sans-serif; margin: 40px; line-height: 1.6; }"
            "        h1 { color: #333; }"
            "        .success { padding: 15px; background-color: #dff0d8; color: #3c763d; }"
            "    </style>"
            "</head>"
            "<body>"
            "    <h1>Form Submitted Successfully</h1>"
            "    <div class='success'>"
            "        <p>Thank you for your submission!</p>"
            "    </div>"
            "    <h2>You submitted:</h2>"
            "    <p><strong>Name:</strong> %s</p>"
            "    <p><strong>Email:</strong> %s</p>"
            "    <p><strong>Message:</strong> %s</p>"
            "    <p><a href='/form'>Back to Form</a></p>"
            "    <p><a href='/'>Back to Home</a></p>"
            "</body>"
            "</html>",
            name, email, message);
        
        w.Write(response);
    }
    
    return 0;
}
```

Register it in `main.c`:

```c
http.HandleFunc("/form", FormHandler);
```

## Example 5: Serving a Static File

To serve a file from disk:

```c
int ServeFile(ResponseWriter w, Request r, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        // File not found
        const char* not_found =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "<html><body><h1>404 Not Found</h1><p>The requested file could not be found.</p></body></html>";
        
        w.Write(not_found);
        return -1;
    }
    
    // Determine file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Read the entire file
    char* buffer = (char*)malloc(file_size + 1);
    if (!buffer) {
        fclose(file);
        w.Write("HTTP/1.1 500 Internal Server Error\r\n\r\nMemory allocation failed");
        return -1;
    }
    
    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';
    fclose(file);
    
    // Determine content type based on file extension
    const char* content_type = "text/plain";
    char* extension = strrchr(filename, '.');
    if (extension) {
        if (strcmp(extension, ".html") == 0 || strcmp(extension, ".htm") == 0) {
            content_type = "text/html";
        } else if (strcmp(extension, ".css") == 0) {
            content_type = "text/css";
        } else if (strcmp(extension, ".js") == 0) {
            content_type = "application/javascript";
        } else if (strcmp(extension, ".jpg") == 0 || strcmp(extension, ".jpeg") == 0) {
            content_type = "image/jpeg";
        } else if (strcmp(extension, ".png") == 0) {
            content_type = "image/png";
        } else if (strcmp(extension, ".gif") == 0) {
            content_type = "image/gif";
        }
    }
    
    // Create and send the response
    char header[512];
    snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "\r\n",
        content_type, file_size);
    
    // Write the header
    w.Write(header);
    
    // Write the file content
    w.Write(buffer);
    
    free(buffer);
    return 0;
}

// Example usage in a handler
int StaticFileHandler(ResponseWriter w, Request r) {
    // Extract the filename from the path
    // For security, restrict to a specific directory
    const char* base_path = "./static";
    char filename[512];
    
    // Sanitize the path to prevent directory traversal
    // Remove any ".." components that could allow escaping the base directory
    char* path = r.path;
    if (strstr(path, "..") != NULL) {
        w.Write("HTTP/1.1 403 Forbidden\r\n\r\nInvalid path");
        return -1;
    }
    
    // Construct the path
    if (strcmp(path, "/static") == 0 || strcmp(path, "/static/") == 0) {
        // Default to index.html
        snprintf(filename, sizeof(filename), "%s/index.html", base_path);
    } else {
        // Remove the '/static' prefix
        const char* file_path = path + 7; // Skip "/static"
        snprintf(filename, sizeof(filename), "%s%s", base_path, file_path);
    }
    
    return ServeFile(w, r, filename);
}
```

Register it in `main.c`:

```c
http.HandleFunc("/static", StaticFileHandler);
```

## Example 6: Creating a Simple REST API

To create a basic JSON API:

```c
int ApiHandler(ResponseWriter w, Request r) {
    // Determine which API endpoint is being requested
    if (strcmp(r.path, "/api/users") == 0) {
        // Return a list of users
        const char* json =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"
            "{"
            "  \"users\": ["
            "    {\"id\": 1, \"name\": \"John Doe\", \"email\": \"john@example.com\"},"
            "    {\"id\": 2, \"name\": \"Jane Smith\", \"email\": \"jane@example.com\"},"
            "    {\"id\": 3, \"name\": \"Bob Johnson\", \"email\": \"bob@example.com\"}"
            "  ]"
            "}";
        
        w.Write(json);
    } 
    else if (strncmp(r.path, "/api/users/", 11) == 0) {
        // Extract the user ID from the path
        int user_id = atoi(r.path + 11);
        
        // Simulate looking up a user by ID
        char* name;
        char* email;
        
        switch (user_id) {
            case 1:
                name = "John Doe";
                email = "john@example.com";
                break;
            case 2:
                name = "Jane Smith";
                email = "jane@example.com";
                break;
            case 3:
                name = "Bob Johnson";
                email = "bob@example.com";
                break;
            default:
                // User not found
                const char* not_found =
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Type: application/json\r\n"
                    "\r\n"
                    "{\"error\": \"User not found\"}";
                
                w.Write(not_found);
                return 0;
        }
        
        // Return the user information
        char json[512];
        snprintf(json, sizeof(json),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"
            "{\"id\": %d, \"name\": \"%s\", \"email\": \"%s\"}",
            user_id, name, email);
        
        w.Write(json);
    } 
    else {
        // Unknown API endpoint
        const char* not_found =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"
            "{\"error\": \"API endpoint not found\"}";
        
        w.Write(not_found);
    }
    
    return 0;
}
```

Register the API handler in `main.c`:

```c
http.HandleFunc("/api", ApiHandler);
```

## Example 7: Logging Requests

Create a simple logging middleware to track all requests:

```c
// Function to get the current time as a string
void get_current_time(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", tm_info);
}

// Wrapper handler that logs all requests before processing
int LoggingHandler(ResponseWriter w, Request r, HandlerFunc next_handler) {
    char time_str[64];
    get_current_time(time_str, sizeof(time_str));
    
    printf("[%s] %s %s\n", time_str, r.method, r.path);
    
    // Call the next handler in the chain
    return next_handler(w, r);
}

// Example of using the logging handler
int HomePage(ResponseWriter w, Request r) {
    w.Write("Hello, World!");
    return 0;
}

// In main.c, you would set it up like this:
void setup_routes() {
    // Create function pointers to the wrapped handlers
    http.HandleFunc("/", HomePage);
    
    // Then in your main request handling loop, you'd call:
    // LoggingHandler(w, r, appropriate_handler_for_path);
}
```

## Example 8: Basic Authentication

Add simple authentication to your server:

```c
// Simple username/password checking function
int check_credentials(const char* auth_header) {
    // Check if the auth header exists and starts with "Basic "
    if (!auth_header || strncmp(auth_header, "Basic ", 6) != 0) {
        return 0;
    }
    
    // Get the base64 encoded part
    const char* encoded = auth_header + 6;
    
    // Decode Base64 (simplified for this example)
    // In a real app, use a proper Base64 decode function
    // For this example, assume "admin:password" is encoded as "YWRtaW46cGFzc3dvcmQ="
    
    if (strcmp(encoded, "YWRtaW46cGFzc3dvcmQ=") == 0) {
        return 1; // Credentials match
    }
    
    return 0; // Credentials don't match
}

int AuthProtectedHandler(ResponseWriter w, Request r) {
    // Look for the Authorization header
    const char* auth_header = NULL;
    for (int i = 0; i < r.header_count; i++) {
        if (strcmp(r.headers[i].name, "Authorization") == 0) {
            auth_header = r.headers[i].value;
            break;
        }
    }
    
    // Check if credentials are valid
    if (!auth_header || !check_credentials(auth_header)) {
        // If no credentials or invalid credentials, request authentication
        const char* auth_required =
            "HTTP/1.1 401 Unauthorized\r\n"
            "WWW-Authenticate: Basic realm=\"Secure Area\"\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "<html><body><h1>401 Unauthorized</h1><p>Authentication required.</p></body></html>";
        
        w.Write(auth_required);
        return 0;
    }
    
    // If we get here, the user is authenticated
    const char* success =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<html><body>"
        "<h1>Secure Area</h1>"
        "<p>You have successfully accessed the protected area.</p>"
        "</body></html>";
    
    w.Write(success);
    return 0;
}
```

Register the protected handler in `main.c`:

```c
http.HandleFunc("/admin", AuthProtectedHandler);
```

These examples should help you get started with extending the basic C HTTP server for various common web development scenarios. 