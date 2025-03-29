#include "header.h"

const char* req_form = 
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html\r\n"
"\r\n"
"<!DOCTYPE html>"
"<html>"
"<head>"
"    <title>C HTTP Server Form</title>"
"    <style>"
"        body { font-family: Arial, sans-serif; max-width: 600px; margin: 0 auto; padding: 20px; }"
"        .form-group { margin-bottom: 15px; }"
"        label { display: block; margin-bottom: 5px; font-weight: bold; }"
"        input[type='text'], input[type='email'], textarea {"
"            width: 100%; padding: 8px; border: 1px solid #ddd; border-radius: 4px;"
"            box-sizing: border-box;"
"        }"
"        button {"
"            background-color: #4CAF50; color: white; padding: 10px 15px;"
"            border: none; border-radius: 4px; cursor: pointer;"
"        }"
"        button:hover { background-color: #45a049; }"
"        .response { margin-top: 20px; padding: 10px; border-radius: 4px; }"
"        .success { background-color: #dff0d8; color: #3c763d; }"
"        .error { background-color: #f2dede; color: #a94442; }"
"    </style>"
"</head>"
"<body>"
"    <h1>Sample Form</h1>"
"    <form method='POST' action='/'>"
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
"        <button type='submit'>Submit</button>"
"    </form>"
"    %s"  // This will be replaced with the response message
"</body>"
"</html>";

int handle(Router router, Client client) {
    char buffer[8192];
    ssize_t bytes_read = read(client.socket, buffer, sizeof(buffer) - 1);

    if (bytes_read < 0) {
        perror("read() failed");
        close(client.socket);
        return -1;
    }
    buffer[bytes_read] = '\0';

    Request* req = parse_http_request(buffer);
    if (!req) {
        perror("Failed to parse HTTP request");
        close(client.socket);
        return -1;
    }

    // Prepare response based on the request
    char response[8192];
    char response_message[512] = "";

    if (strcmp(req->path, "/") == 0) {
        if (strcmp(req->method, "POST") == 0) {
            // Process form submission
            printf("Form submitted with data:\n%s\n", req->body);
            
            // Parse form data (simple key=value&key2=value2 parsing)
            char* saveptr;
            char* token = strtok_r(req->body, "&", &saveptr);
            char form_data[3][256] = {0}; // name, email, message
            
            while (token != NULL) {
                char* eq = strchr(token, '=');
                if (eq) {
                    *eq = '\0';
                    char* key = token;
                    char* value = eq + 1;
                    
                    // URL decode the value (very simple version)
                    char decoded[256];
                    int j = 0;
                    for (int i = 0; value[i] && j < 255; i++) {
                        if (value[i] == '+') {
                            decoded[j++] = ' ';
                        } else if (value[i] == '%' && isxdigit(value[i+1]) && isxdigit(value[i+2])) {
                            char hex[3] = {value[i+1], value[i+2], '\0'};
                            decoded[j++] = (char)strtol(hex, NULL, 16);
                            i += 2;
                        } else {
                            decoded[j++] = value[i];
                        }
                    }
                    decoded[j] = '\0';
                    
                    if (strcmp(key, "name") == 0) strncpy(form_data[0], decoded, sizeof(form_data[0]));
                    else if (strcmp(key, "email") == 0) strncpy(form_data[1], decoded, sizeof(form_data[1]));
                    else if (strcmp(key, "message") == 0) strncpy(form_data[2], decoded, sizeof(form_data[2]));
                }
                token = strtok_r(NULL, "&", &saveptr);
            }
            
            // Create success message
            snprintf(response_message, sizeof(response_message),
                "<div class='response success'>"
                "<h3>Thank you for your submission!</h3>"
                "<p><strong>Name:</strong> %s</p>"
                "<p><strong>Email:</strong> %s</p>"
                "<p><strong>Message:</strong> %s</p>"
                "</div>",
                form_data[0], form_data[1], form_data[2]);
        }
        
        // Serve the form (with response message if this was a POST)
        snprintf(response, sizeof(response), req_form, response_message);
    } else {
        // 404 Not Found
        snprintf(response, sizeof(response),
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "<html><body style='height:100vh;display:flex;justify-content:center;align-items:center;'>"
            "<h1>404 Not Found</h1>"
            "<p>The requested path '%s' was not found on this server.</p>"
            "</body></html>",
            req->path);
    }

    ssize_t bytes_written = write(client.socket, response, strlen(response));
    if (bytes_written < 0) {
        perror("write() failed");
    }

    free_request(req);
    close(client.socket);
    return 0;
}
