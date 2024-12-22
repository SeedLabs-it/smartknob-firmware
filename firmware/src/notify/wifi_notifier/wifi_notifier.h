#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include <functional>
#include "configuration.h"

enum WiFiCommandType
{
    RequestAP,
    RequestSTA,
    RequestRedirect
};

enum RedirectPage
{
    REDIRECT_MQTT,
    REDIRECT_SPOTIFY,
    DONE_MQTT,
    DONE_SPOTIFY
};

union WiFiCommandBody
{
    WiFiConfiguration wifi_sta_config;
    RedirectPage redirect_page;
};

struct WiFiCommand
{
    WiFiCommandType type;
    WiFiCommandBody body;
};

typedef std::function<void(WiFiCommand)>
    WiFiNotifierCallback;

class WiFiNotifier
{
public:
    WiFiNotifier();
    void requestAP();
    void requestSTA(WiFiConfiguration wifi_config);
    void loopTick();
    void setCallback(WiFiNotifierCallback callback);
    void redirect(RedirectPage page);

private:
    QueueHandle_t wifi_notifications_queue;
    WiFiNotifierCallback callback;
    WiFiCommand recieved_command;
};