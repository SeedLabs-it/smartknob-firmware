#include "async_http.h"

AsyncHttp::AsyncHttp(AsyncClient *client, const char *host, const uint16_t port) : client_(client), host_(host), port_(port)
{
    assert(client_ != nullptr);
}

AsyncHttp::~AsyncHttp()
{
}

void AsyncHttp::get(const char *path, AcDataHandler cb, void *context)
{
    if (!client_->connected())
    {
        if (client_->connect(host_, port_))
        {
        }
        else
        {
            LOGE("Connection failed to server %s:%d", host_, port_);
            return; // TODO: handle error
        }
    }

    char *request = (char *)heap_caps_malloc(512, MALLOC_CAP_SPIRAM);
    snprintf(request, 512, "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: ESP\r\nConnection: close\r\n\r\n", path, host_);

    client_->onConnect([this, request, cb, context](void *arg, AsyncClient *client)
                       { 
                        size_t len = client_->write(request);
                        // TODO verify if all bytes were written and or handle errors etc.
                        client->onData(cb, context); });

    client_->onDisconnect([this, request](void *arg, AsyncClient *client)
                          { 
                            LOGV(LOG_LEVEL_DEBUG,"Disconnected from server %s:%d", host_, port_);
                            client_->close();
                            heap_caps_free(request); });
}
