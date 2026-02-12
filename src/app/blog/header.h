#ifndef BLOG_H
#define BLOG_H

#include "http/header.h"

int BlogIndex(ResponseWriter* rw, Request* r);

Router* BlogRouter();

#endif  // BLOG_H
