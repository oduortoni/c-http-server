#include "header.h"

HttpServer http = {
  .ListenAndServe = listenAndServe,
  .HandleFunc = handleFunc,
};

int listenAndServe(char *host, Router *router) {
  if(router == NULL) {
    router = http.router;
  }
  puts("http/server.c");
  for(size_t i = 0; i < ARRAY_LEN(router->patterns) && router->patterns[i]; i++) {
    printf("SERVERT: %s\n", router->patterns[i]);
  }
  // printf("RouteD %s\n", router->patterns[1]);
  Processor processor = {handle_connection, router};
  serve(host, processor);
  return 0;
}
