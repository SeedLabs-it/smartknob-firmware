#include "mqtt_notifier.h"

MqttNotifier::MqttNotifier()
{
    mqtt_notifications_queue = xQueueCreate(5, sizeof(MqttCommand));
    assert(mqtt_notifications_queue != NULL);
}

void MqttNotifier::setCallback(MqttNotifierCallback callback)
{
    this->callback = callback;
}

void MqttNotifier::requestConnect(MQTTConfiguration mqtt_config)
{
    MqttCommand command;
    command.type = RequestConnect;
    command.body.mqtt_config = mqtt_config;

    xQueueSendToBack(mqtt_notifications_queue, &command, 0);
}

void MqttNotifier::requestDisconnect()
{
    MqttCommand command;
    command.type = RequestDisconnect;

    xQueueSendToBack(mqtt_notifications_queue, &command, 0);
}

void MqttNotifier::requestReconnect()
{
    MqttCommand command;
    command.type = RequestReconnect;

    xQueueSendToBack(mqtt_notifications_queue, &command, 0);
}

void MqttNotifier::requestInit()
{
    MqttCommand command;
    command.type = RequestInit;

    xQueueSendToBack(mqtt_notifications_queue, &command, 0);
}

void MqttNotifier::requestReset()
{
    MqttCommand command;
    command.type = RequestReset;

    xQueueSendToBack(mqtt_notifications_queue, &command, 0);
}

void MqttNotifier::loopTick()
{
    if (xQueueReceive(mqtt_notifications_queue, &recieved_command, 0) == pdTRUE)
    {
        callback(recieved_command);
    }
}