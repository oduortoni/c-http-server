# Integration tests for C HTTP Server

# Test: Index page
GET http://localhost:{{PORT}}/
HTTP 200

# Test: About page
GET http://localhost:{{PORT}}/about
HTTP 200

# Test: Explicit 404 page
GET http://localhost:{{PORT}}/404
HTTP 404

# Test: Garbage URL (should return 404)
GET http://localhost:{{PORT}}/garbage-url-asdfa123
HTTP 404

# Test: Static file that exists
GET http://localhost:{{PORT}}/static/index.html
HTTP 200

# Test: Static file that doesn't exist (should return 404)
GET http://localhost:{{PORT}}/static/nonexistent.js
HTTP 404

# Test: Static file that exist (should return 200)
GET http://localhost:{{PORT}}/static/index.js
HTTP 200
