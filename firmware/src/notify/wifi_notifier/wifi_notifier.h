#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include <functional>

// typedef void MotorUpdaterCallback(PB_SmartKnobConfig config);
typedef std::function<void(void)> WiFiNotifierCallback;

class WiFiNotifier
{
public:
    WiFiNotifier();
    // enqueues new motor config to be applied
    void requestAP();
    // pull one message from the queue and apply with callback
    void loopTick();
    // set callbacks
    void setCallback(WiFiNotifierCallback callback);

private:
    QueueHandle_t wifi_notifications_queue;
    WiFiNotifierCallback callback;
    uint8_t recieved_command;
};