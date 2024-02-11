#include "motor_updater.h"

MotorUpdater::MotorUpdater(MotorUpdaterCallback callback)
{
    this->callback = callback;

    motor_updates_queue = xQueueCreate(2, sizeof(PB_SmartKnobConfig));
    assert(motor_updates_queue != NULL);
}

void MotorUpdater::requestUpdate(PB_SmartKnobConfig config)
{
    xQueueSendToBack(motor_updates_queue, &config, 0);
}

void MotorUpdater::loopTick()
{
    if (xQueueReceive(motor_updates_queue, &tmp_recieved_config, 0) == pdTRUE)
    {
        callback(tmp_recieved_config);
    }
}