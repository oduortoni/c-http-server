#include "header.h"

int handle_connection(Client client) {
    handle(router, client);
    return 0;
}