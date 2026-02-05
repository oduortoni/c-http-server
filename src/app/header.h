#ifndef _APP_H
#define _APP_H

#include "../lib/http/header.h"

int Index(ResponseWriter* w, Request* r);
int About(ResponseWriter* w, Request* r);
int Error404(ResponseWriter* w, Request* r);
int Static(ResponseWriter* w, Request* r);

#endif
