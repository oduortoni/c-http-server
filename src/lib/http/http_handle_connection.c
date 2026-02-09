#include "header.h"

// the request_len is necessary for protocols that do not use the null
// terminator
ProtocolResponse
http_handle_connection(RequestContext* context, const char* request_data,
                       [[maybe_unused]] size_t request_len)
{
        (void)request_len;
        Router* router        = (Router*)context->router;

        HttpResponse response = http_handle(router, request_data);

        // convert HttpResponse to ProtocolResponse
        return (ProtocolResponse){.data   = response.data,
                                  .length = response.length,
                                  .status = response.status};
}
