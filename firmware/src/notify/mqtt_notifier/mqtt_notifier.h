#pragma once
#include "configuration.h"

enum MqttCommandType
{
    RequestSetupConnect,
    // RequestDisconnect,
    // RequestReconnect,
    // RequestInit,
    // RequestReset,
};

union MqttCommandBody
{
    MQTTConfiguration mqtt_config;
};

struct MqttCommand
{
    MqttCommandType type;
    MqttCommandBody body;
};

typedef std::function<void(MqttCommand)> MqttNotifierCallback;

class MqttNotifier
{
public:
    MqttNotifier();
    void requestSetupAndConnect(MQTTConfiguration mqtt_config);
    // void requestDisconnect();
    // void requestReconnect();
    // void requestInit();
    // void requestReset();
    void loopTick();
    void setCallback(MqttNotifierCallback callback);

private:
    QueueHandle_t mqtt_notifications_queue;
    MqttNotifierCallback callback;
    MqttCommand recieved_command;
};