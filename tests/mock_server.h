#ifndef CGPTCHATBOT_MOCK_SERVER_H
#define CGPTCHATBOT_MOCK_SERVER_H
#include "microhttpd.h"
typedef struct {
    const char *url;
    const char *response_body;
    int status_code;
} MockServerRequestHandlerData;

MockServerRequestHandlerData *ms_create_handler_data(const char *url, const char *response_body, int status_code);
void ms_free_handler_data(MockServerRequestHandlerData *data);

struct MHD_Daemon *create_mock_server(MockServerRequestHandlerData **handler_data);
#endif //CGPTCHATBOT_MOCK_SERVER_H
