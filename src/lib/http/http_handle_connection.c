#include "header.h"

ProtocolResponse
http_handle_connection(RequestContext* context, const char* request_data,
                       size_t request_len)
{
        Router* router        = (Router*)context->router;

        // HTTP layer: Process request (pure function)
        HttpResponse response = http_handle(router, request_data);

        // Convert HttpResponse to ProtocolResponse
        return (ProtocolResponse){.data   = response.data,
                                  .length = response.length,
                                  .status = response.status};
}
