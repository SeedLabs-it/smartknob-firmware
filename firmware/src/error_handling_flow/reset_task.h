#pragma once

#include "logger.h"
#include "task.h"
#include "esp_ota_ops.h"
#include "configuration.h"
#include "motor_foc/motor_task.h"
#include "../notify/motor_notifier/motor_notifier.h"
#include "../events/events.h"

class ResetTask : public Task<ResetTask>
{
    friend class Task<ResetTask>; // Allow base Task to invoke protected run()

public:
    ResetTask(const uint8_t task_core, Configuration &configuration);
    ~ResetTask();

    void softReset();
    void hardReset();

    void setMotorTask(MotorTask *motor_task);

    void setLogger(Logger *logger);
    void setVerbose(bool verbose);
    void toggleVerbose();

    void setSharedEventsQueue(QueueHandle_t shared_event_queue);
    void publishEvent(WiFiEvent event);

protected:
    void run();

private:
    Configuration &configuration_;
    MotorTask *motor_task_;
    char buf_[128];

    QueueHandle_t shared_events_queue;
};
