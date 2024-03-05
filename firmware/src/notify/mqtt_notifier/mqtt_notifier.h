#pragma once
#include "configuration.h"

enum MqttCommandType
{
    RequestConnect,
    RequestDisconnect,
    RequestReconnect,
    RequestInit,
    RequestReset,
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
    // enqueues new request to connect to mqtt
    void requestConnect(MQTTConfiguration mqtt_config);
    // enqueues new request to disconnect from mqtt
    void requestDisconnect();
    // enqueues new request to reconnect to mqtt
    void requestReconnect();
    // enqueues new request to initialize mqtt
    void requestInit();
    // enqueues new request to reset mqtt
    void requestReset();
    // pull one message from the queue and apply with callback
    void loopTick();
    // set callbacks
    void setCallback(MqttNotifierCallback callback);

private:
    QueueHandle_t mqtt_notifications_queue;
    MqttNotifierCallback callback;
    MqttCommand recieved_command;
};