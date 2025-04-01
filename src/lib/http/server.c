#include "header.h"

HttpServer http = {listenAndServe, handleFunc};

int listenAndServe(char *host, Router *router) {
  if(router == NULL) {
    router = http.router;
  }
  puts("http/server.c");
  for(int i = 0; i < 4; i++) {
    printf("SERVERT: %s\n", router->patterns[i]);
  }
  // printf("RouteD %s\n", router->patterns[1]);
  Processor processor = {handle_connection, router};
  serve(host, processor);
  return 0;
}
