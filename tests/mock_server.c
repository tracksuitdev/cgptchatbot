#include "mock_server.h"
#include <stdio.h>
#include <string.h>
#include <microhttpd.h>
#include <stdlib.h>

enum MHD_Result request_handler(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls) {
    MockServerRequestHandlerData **handlers = (MockServerRequestHandlerData **) cls;
    for (int i = 0; handlers[i] != NULL; i++) {
        MockServerRequestHandlerData *data = handlers[i];
        const char *response_body = data->response_body;
        int status_code = data->status_code;
        struct MHD_Response *http_response;
        enum MHD_Result ret;

        if (strcmp(url, data->url) == 0) {
            // Create the HTTP response
            http_response = MHD_create_response_from_buffer(strlen(response_body),
                                                            (void *) response_body,
                                                            MHD_RESPMEM_PERSISTENT);
            if (!http_response)
                return MHD_NO;

            // Send the response
            ret = MHD_queue_response(connection, status_code, http_response);
            MHD_destroy_response(http_response);

            return ret;
        }
    }
    struct MHD_Response *http_response = MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
    enum MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, http_response);
    MHD_destroy_response(http_response);

    return ret;
}

MockServerRequestHandlerData *ms_create_handler_data(const char *url, const char *response_body, int status_code) {
    MockServerRequestHandlerData *data = malloc(sizeof(MockServerRequestHandlerData));
    data->url = strdup(url);
    data->response_body = strdup(response_body);
    data->status_code = status_code;
    return data;
}

void ms_free_handler_data(MockServerRequestHandlerData *data) {
    free((void *) data->url);
    free((void *) data->response_body);
    free(data);
}

struct MHD_Daemon *create_mock_server(MockServerRequestHandlerData **handler_data) {


    // Start the mock server
    struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, 8080, NULL, NULL,
                                                 &request_handler, handler_data, MHD_OPTION_END);
    if (!daemon) {
        fprintf(stderr, "Failed to start the server\n");
        return NULL;
    }

    return daemon;
}
