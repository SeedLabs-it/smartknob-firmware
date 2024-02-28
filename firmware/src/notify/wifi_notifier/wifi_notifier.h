#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include <functional>
#include "configuration.h"

enum WiFiCommandType
{
    RequestAP,
    RequestSTA,
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
    // enqueues new request to start wifi AP
    void requestAP();
    // enqieies new request to start wifi STA
    void requestSTA(WiFiConfiguration wifi_config);
    // pull one message from the queue and apply with callback
    void loopTick();
    // set callbacks
    void setCallback(WiFiNotifierCallback callback);

private:
    QueueHandle_t wifi_notifications_queue;
    WiFiNotifierCallback callback;
    WiFiCommand recieved_command;
};