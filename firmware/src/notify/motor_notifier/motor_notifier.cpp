#include "motor_notifier.h"

MotorNotifier::MotorNotifier(MotorUpdaterCallback callback)
{
    this->callback = callback;

    motor_updates_queue = xQueueCreate(5, sizeof(PB_SmartKnobConfig));
    assert(motor_updates_queue != NULL);
}

void MotorNotifier::requestUpdate(PB_SmartKnobConfig config)
{
    xQueueSendToBack(motor_updates_queue, &config, 0);
}

void MotorNotifier::loopTick()
{
    if (xQueueReceive(motor_updates_queue, &tmp_recieved_config, 0) == pdTRUE)
    {
        callback(tmp_recieved_config);
    }
}