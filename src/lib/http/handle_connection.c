#include "header.h"

int handle_connection(void *components, Client client) {
  Router *router = (Router*)components;
  puts("http/handle_connection.c");
  for(int i = 0; i < 4; i++) {
    printf("HCRT: %s\n", router->patterns[i]);
  }
  printf("PROCESSOR COMPONENTS: %s\n", router->patterns[1]);
  handle(router, client);
  return 0;
}
