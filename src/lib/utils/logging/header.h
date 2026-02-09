#ifndef INCLUDE_LOGGING_HEADER_H_
#define INCLUDE_LOGGING_HEADER_H_

#define log__internal(stream, mode, ...)                             \
        do {                                                         \
                fprintf(stream, mode ":\033[0m%s:%d:\n\t", __FILE__, \
                        __LINE__);                                   \
                fprintf(stream, __VA_ARGS__);                        \
                fprintf(stream, "\n");                               \
        } while (0)

#define debug(...) log__internal(stdout, "\033[35mDEBUG", __VA_ARGS__)
#define info(...) log__internal(stdout, "\033[32mINFO", __VA_ARGS__)
#define error(...) log__internal(stderr, "\033[31mERROR", __VA_ARGS__)

#endif  // INCLUDE_LOGGING_HEADER_H_
