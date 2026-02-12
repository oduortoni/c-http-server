#ifndef _APP_H
#define _APP_H

#include "http/header.h"

#define TEMPLATE_PATH "src/app/templates/"

int Index(ResponseWriter* w, Request* r);
int About(ResponseWriter* w, Request* r);
int Error404(ResponseWriter* w, Request* r);
int Static(ResponseWriter* w, Request* r);
int TestHandler(ResponseWriter* w, Request* r);

#endif
