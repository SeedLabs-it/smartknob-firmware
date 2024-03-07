#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include <functional>
#include "configuration.h"

enum WiFiCommandType
{
    RequestAP,
    RequestSTA,
    RequestRetryMQTT,
};

union WiFiCommandBody
{
    WiFiConfiguration wifi_sta_config;
};

struct WiFiCommand
{
    WiFiCommandType type;
    WiFiCommandBody body;
};

typedef std::function<void(WiFiCommand)> WiFiNotifierCallback;

class WiFiNotifier
{
public:
    WiFiNotifier();
    void requestAP();
    void requestSTA(WiFiConfiguration wifi_config);
    void requestRetryMQTT();
    void loopTick();
    void setCallback(WiFiNotifierCallback callback);

private:
    QueueHandle_t wifi_notifications_queue;
    WiFiNotifierCallback callback;
    WiFiCommand recieved_command;
};