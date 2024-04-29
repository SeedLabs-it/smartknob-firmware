#pragma once

#include "logger.h"
#include "task.h"
#include "app_config.h"
#include <vector>
#include <Adafruit_VL53L0X.h>

#include "driver/temp_sensor.h"

const uint16_t PROXIMITY_SENSOR_OFFSET_MM = 10;

struct StrainCalibration
{
    float idle_value;
    float press_delta;
};

class SensorsTask : public Task<SensorsTask>
{
    friend class Task<SensorsTask>; // Allow base Task to invoke protected run()

public:
    SensorsTask(const uint8_t task_core);
    ~SensorsTask();

    void addStateListener(QueueHandle_t queue);
    void updateStrainCalibration(float idle_value, float press_delta);

protected:
    void run();

private:
    QueueHandle_t sensors_state_queue_;

    std::vector<QueueHandle_t> state_listeners_;

    SemaphoreHandle_t mutex_;
    void publishState(const SensorsState &state);
    StrainCalibration strain_calibration;
};
