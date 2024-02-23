#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include <functional>
#include "configuration.h"

typedef std::function<void(OSMode)> OSConfigNotifierCallback;

class OSConfigNotifier
{
public:
    OSConfigNotifier();
    void setOSMode(OSMode os_mode);
    void loopTick();
    void setCallback(OSConfigNotifierCallback callback);

private:
    QueueHandle_t notifications_queue;
    OSConfigNotifierCallback callback;
    OSMode recieved_command;
};