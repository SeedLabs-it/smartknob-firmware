#pragma once
#include <cJSON.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <logging.h>

struct HttpResponseState
{
    char *resp_buffer = nullptr;
    char *body_start = nullptr;
    size_t resp_size = 0;
    size_t content_length = 0;
    bool headers_parsed = false;
    bool body_processed = false;
};

using ResponseCallback = std::function<void(const char *, size_t)>;

class AsyncHttp
{
public:
    // using ResponseCallback = std::function<void(char *)>;

    AsyncHttp(AsyncClient *client, const char *host, const uint16_t port);
    ~AsyncHttp();

    void get(const char *path, const char *expected_content_type, ResponseCallback rc);
    // void get(const char *path, AcDataHandler cb, void *context);
    void post(const char *path, const char *headers, const char *body);

    void handleHttpResponse(
        void *arg,
        AsyncClient *client,
        void *data,
        size_t len,
        const char *expected_content_type,
        ResponseCallback rc);

private:
    HttpResponseState state_;

    AsyncClient *client_ = nullptr;
    const char *host_ = nullptr;
    const uint16_t port_;

    void connect_(const char *path, const char *port);

    bool appendData(const void *data, size_t len);
    bool parseHeaders(const char *expected_content_type);
    bool isBodyComplete();
    void resetHttpResponseState();
};
