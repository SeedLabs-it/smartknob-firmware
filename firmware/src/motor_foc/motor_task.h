#pragma once

#include <Arduino.h>
#include <SimpleFOC.h>
#include <vector>

#include "../configuration.h"
#include "../proto/proto_gen/smartknob.pb.h"
#include "../task.h"

#include <PciManager.h>
#include <PciListenerImp.h>

enum class CommandType
{
    CALIBRATE,
    CONFIG,
    HAPTIC,
};

struct HapticData
{
    bool press;
    bool long_press;
};

struct Command
{
    CommandType command_type;
    union CommandData
    {
        uint8_t unused;
        PB_SmartKnobConfig config;
        HapticData haptic;
    };
    CommandData data;
};

class MotorTask : public Task<MotorTask>
{
    friend class Task<MotorTask>; // Allow base Task to invoke protected run()

public:
    MotorTask(const uint8_t task_core, Configuration &configuration);
    ~MotorTask();

    void setConfig(const PB_SmartKnobConfig config);
    void playHaptic(bool press, bool long_press);
    void runCalibration();

    void addListener(QueueHandle_t queue);

protected:
    void run();

private:
    Configuration &configuration_;
    QueueHandle_t queue_;
    std::vector<QueueHandle_t> listeners_;
    char buf_[72];

    // BLDC motor & driver instance
    BLDCMotor motor = BLDCMotor(7, 5.4, 300);
    BLDCDriver6PWM driver = BLDCDriver6PWM(PIN_UH, PIN_UL, PIN_VH, PIN_VL, PIN_WH, PIN_WL);

    void publish(const PB_SmartKnobState &state);
    void calibrate();
    void checkSensorError();
};
