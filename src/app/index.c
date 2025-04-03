#include "header.h"

const char* html = 
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
"%s"  // This will be replaced with the response message
"</body>"
"</html>";

const char *r_message = 
"<div class='response success'>"
"<h3>Thank you for your submission!</h3>"
"<p><strong>Name:</strong> %s</p>"
"<p><strong>Email:</strong> %s</p>"
"<p><strong>Message:</strong> %s</p>"
"</div>";

int Index(ResponseWriter* w, Request* r) {
    FormData form_data = {0};
    char response_message[512] = "";

    if (strcmp(r->method, "POST") == 0) {
        printf("Form submitted with data:\n%s\n", r->body);
        parse_form_data(r->body, &form_data);
        
        const char* name = get_form_value(&form_data, "name");
        const char* email = get_form_value(&form_data, "email");
        const char* message = get_form_value(&form_data, "message");

        snprintf(response_message, sizeof(response_message), r_message, name, email, message);
    }

    char response[8192];
    snprintf(response, sizeof(response), html, response_message);

    SetStatus(w, 200, "OK");
    SetHeader(w, "Content-Type", "text/html");
    w->WriteString(w, response);
    
    return 0;
}
