#include "header.h"

int handleFunc(char *pattern, HandlerFunc handler) {
    if(!http.router) {
        http.router = (Router*)malloc(sizeof(Router));
    }
    for(int i = 0; i < 50; i++) {
        if(!http.router->patterns[i]) {
            int n = strlen(pattern);
            http.router->patterns[i] = malloc(n+1);
            strncpy(http.router->patterns[i], pattern, n);
            http.router->patterns[i][n] = '\0';
            http.router->handlers[i] = handler;
            break;
        }
    }
    return 0;
}
