#include "header.h"

int http_handle_connection(RequestContext *context, Client client) {
  Router *router = (Router*)context->router;
  puts("http/handle_connection.c");
  for(int i = 0; i < 4; i++) {
    printf("HCRT: %s\n", router->patterns[i]);
  }
  printf("PROCESSOR COMPONENTS: %s\n", router->patterns[1]);
  http_handle(router, client);
  return 0;
}
