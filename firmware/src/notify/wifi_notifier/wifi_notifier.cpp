#include "wifi_notifier.h"

WiFiNotifier::WiFiNotifier()
{

    wifi_notifications_queue = xQueueCreate(5, sizeof(uint8_t));
    assert(wifi_notifications_queue != NULL);
}

void WiFiNotifier::setCallback(WiFiNotifierCallback callback)
{
    this->callback = callback;
}

void WiFiNotifier::requestAP()
{
    uint8_t command = 1;
    xQueueSendToBack(wifi_notifications_queue, &command, 0);
}
void WiFiNotifier::loopTick()
{
    if (xQueueReceive(wifi_notifications_queue, &recieved_command, 0) == pdTRUE)
    {
        callback();
    }
}
