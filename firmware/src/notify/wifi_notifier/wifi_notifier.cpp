#include "wifi_notifier.h"

WiFiNotifier::WiFiNotifier()
{
    wifi_notifications_queue = xQueueCreate(5, sizeof(WiFiCommand));
    assert(wifi_notifications_queue != NULL);
}

void WiFiNotifier::setCallback(WiFiNotifierCallback callback)
{
    this->callback = callback;
}

void WiFiNotifier::requestAP()
{
    WiFiCommand command;
    command.type = RequestAP;

    xQueueSendToBack(wifi_notifications_queue, &command, 0);
}

void WiFiNotifier::requestSTA(WiFiConfiguration wifi_config)
{
    WiFiCommand command;
    command.type = RequestSTA;
    strcpy(command.body.wifi_sta_config.ssid, wifi_config.ssid);
    strcpy(command.body.wifi_sta_config.passphrase, wifi_config.passphrase);
    strcpy(command.body.wifi_sta_config.knob_id, wifi_config.knob_id);

    xQueueSendToBack(wifi_notifications_queue, &command, 0);
}

void WiFiNotifier::loopTick()
{
    if (xQueueReceive(wifi_notifications_queue, &recieved_command, 0) == pdTRUE)
    {
        callback(recieved_command);
    }
}
