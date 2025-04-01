#include "header.h"

int handle_connection(void *components, Client client) {
  Router *router = (Router*)components;
  printf("PROCESSOR COMPONENTS: %s\n", router->patterns[1]);
  handle(router, client);
  return 0;
}
