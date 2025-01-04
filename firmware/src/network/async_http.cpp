#include "async_http.h"

AsyncHttp::AsyncHttp(AsyncClient *client, const char *host, const uint16_t port) : client_(client), host_(host), port_(port)
{
    assert(client_ != nullptr);
}

AsyncHttp::~AsyncHttp()
{
}

void AsyncHttp::get(const char *path, const char *content_type, ResponseCallback rc)
{
    if (!client_->connected())
    {
        if (client_->connect(host_, port_))
        {
            LOGV(LOG_LEVEL_DEBUG, "Connected to server %s:%d", host_, port_);
        }
        else
        {
            LOGE("Connection failed to server %s:%d", host_, port_);
            return; // TODO: handle error
        }
    }

    char request[512];
    snprintf(request, 512, "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: ESP\r\nConnection: close\r\n\r\n", path, host_);

    // HttpResponseState state = {};
    HttpResponseState state;

    client_->onConnect([this, request, rc, content_type](void *arg, AsyncClient *client)
                       {
                           size_t len = client_->write(request);
                           // TODO verify if all bytes were written and or handle errors etc.
                           client->onData([this, rc, content_type](void *arg, AsyncClient *client, void *data, size_t len)
                                          { handleHttpResponse(arg, client, data, len, content_type, rc); }); });

    // client_->onDisconnect([this, request](void *arg, AsyncClient *client)
    //                       {
    //                             LOGV(LOG_LEVEL_DEBUG, "Disconnected from server %s:%d", host_, port_);
    //                             client_->close();
    //                             heap_caps_free(request); });
    client_->onDisconnect([this](void *arg, AsyncClient *client)
                          { resetHttpResponseState(); });
}

bool AsyncHttp::appendData(const void *data, size_t len)
{
    char *newBuffer = (char *)heap_caps_realloc(state_.resp_buffer, state_.resp_size + len + 1, MALLOC_CAP_SPIRAM);
    if (!newBuffer)
    {
        LOGE("Memory allocation failed");
        if (state_.resp_buffer)
            heap_caps_free(state_.resp_buffer);
        state_.resp_buffer = nullptr;
        state_.resp_size = 0;
        return false;
    }
    state_.resp_buffer = newBuffer;
    memcpy(state_.resp_buffer + state_.resp_size, data, len);
    state_.resp_size += len;
    state_.resp_buffer[state_.resp_size] = '\0';
    return true;
}

bool AsyncHttp::parseHeaders(const char *expected_content_type)
{
    if (state_.headers_parsed)
    {
        return true;
    }

    state_.body_start = strstr(state_.resp_buffer, "\r\n\r\n");
    if (!state_.body_start)
    {
        return false;
    }

    char *content_type_start = strstr(state_.resp_buffer, "Content-Type:");
    if (!content_type_start)
    {
        LOGE("No Content-Type found");
        return false;
    }
    content_type_start += 14;
    char *content_type_end = strstr(content_type_start, "\r\n");
    size_t ct_len = content_type_end - content_type_start;
    char *content_type_str = (char *)heap_caps_calloc(1, ct_len + 1, MALLOC_CAP_SPIRAM);
    strncpy(content_type_str, content_type_start, ct_len);
    content_type_str[ct_len] = '\0';

    if (!strstr(content_type_str, expected_content_type))
    {
        LOGE("Invalid Content-Type: %s", content_type_str);
        heap_caps_free(content_type_str);
        return false;
    }
    heap_caps_free(content_type_str);

    char *content_length_str = strstr(state_.resp_buffer, "Content-Length:");
    if (content_length_str)
    {
        state_.content_length = atoi(content_length_str + 15);
    }

    state_.headers_parsed = true;
    return true;
}

bool AsyncHttp::isBodyComplete()
{
    if (!state_.headers_parsed)
        return false;

    // Body starts after \r\n\r\n
    const char *header_end = strstr(state_.resp_buffer, "\r\n\r\n");
    if (!header_end)
        return false;
    size_t received_body_size = state_.resp_size - (header_end + 4 - state_.resp_buffer);
    if (state_.content_length == 0 || received_body_size >= state_.content_length)
    {
        return true;
    }
    return false;
}

void AsyncHttp::resetHttpResponseState()
{
    // resp_buffer reallocated.
    state_.resp_size = 0;
    state_.content_length = 0;
    state_.headers_parsed = false;
    state_.body_processed = false;
    state_.body_start = nullptr;
}

void AsyncHttp::handleHttpResponse(
    void *arg,
    AsyncClient *client,
    void *data,
    size_t len,
    const char *expected_content_type,
    ResponseCallback rc)
{
    if (!appendData(data, len))
    {
        client->close(true);
        return;
    }

    if (!state_.headers_parsed)
    {
        if (!parseHeaders(expected_content_type))
        {
            // Headers not fully parsed or invalid. // TODO: handle
            return;
        }
    }

    if (isBodyComplete() && !state_.body_processed)
    {
        state_.body_processed = true;
        const char *header_end = strstr(state_.resp_buffer, "\r\n\r\n");
        const char *body_ptr = header_end + 4;
        size_t body_size = state_.resp_buffer + state_.resp_size - body_ptr;
        rc(body_ptr, body_size);
        client->close();
    }
}