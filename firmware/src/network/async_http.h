#pragma once
#include <cJSON.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <logging.h>

class AsyncHttp
{
public:
    // using ResponseCallback = std::function<void(char *)>;

    AsyncHttp(AsyncClient *client, const char *host, const uint16_t port);
    ~AsyncHttp();

    void get(const char *path, AcDataHandler cb, void *context);
    void post(const char *path, const char *headers, const char *body);

private:
    AsyncClient *client_ = nullptr;
    const char *host_ = nullptr;
    const uint16_t port_;

    void connect_(const char *path, const char *port);
};
