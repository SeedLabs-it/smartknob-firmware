#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include "../../proto_gen/smartknob.pb.h"
#include <functional>
#include <logging.h>

// typedef void MotorUpdaterCallback(PB_SmartKnobConfig config);
typedef std::function<void(PB_SmartKnobConfig)> MotorUpdaterCallback;

class MotorNotifier
{
public:
    MotorNotifier(MotorUpdaterCallback callback);
    // enqueues new motor config to be applied
    void requestUpdate(PB_SmartKnobConfig config);
    // pull one message from the queue and apply with callback
    void loopTick();

private:
    QueueHandle_t motor_updates_queue;
    MotorUpdaterCallback callback;
    PB_SmartKnobConfig tmp_recieved_config;
};