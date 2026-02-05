# Integration tests for C HTTP Server

# Test: Index page
GET http://localhost:9000/
HTTP 200

# Test: About page
GET http://localhost:9000/about
HTTP 200

# Test: Explicit 404 page
GET http://localhost:9000/404
HTTP 404

# Test: Garbage URL (should return 404)
GET http://localhost:9000/garbage-url-asdfa123
HTTP 404

# Test: Static file that exists
GET http://localhost:9000/static/index.html
HTTP 200

# Test: Static file that doesn't exist (should return 404)
GET http://localhost:9000/static/index.js
HTTP 404
