#include "os_config_notifier.h"

OSConfigNotifier::OSConfigNotifier()
{
    notifications_queue = xQueueCreate(5, sizeof(OSMode));
    assert(notifications_queue != NULL);
}

void OSConfigNotifier::setCallback(OSConfigNotifierCallback callback)
{
    this->callback = callback;
}

void OSConfigNotifier::setOSMode(OSMode os_mode)
{
    xQueueSendToBack(notifications_queue, &os_mode, 0);
}

void OSConfigNotifier::loopTick()
{
    if (xQueueReceive(notifications_queue, &recieved_command, 0) == pdTRUE)
    {
        callback(recieved_command);
    }
}
