#pragma once

#include "logger.h"
#include "task.h"
#include "app_config.h"
#include <vector>
#include <Adafruit_VL53L0X.h>

#if SK_STRAIN
#include <HX711.h>
#endif

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
    SensorsTask(const uint8_t task_core, Configuration *configuration);
    ~SensorsTask();

    void addStateListener(QueueHandle_t queue);
    void strainCalibrationCallback();
    void updateStrainCalibration(float idle_value, float press_delta);
    void factoryStrainCalibrationCallback();
    void weightMeasurementCallback();

protected:
    void run();

private:
    SensorsState sensors_state = {};
    QueueHandle_t sensors_state_queue_;

    std::vector<QueueHandle_t> state_listeners_;

    SemaphoreHandle_t mutex_;
    void publishState(const SensorsState &state);
    StrainCalibration strain_calibration;
    HX711 strain;

    Configuration *configuration_;

    uint8_t strain_calibration_step_ = 0;
    uint8_t factory_strain_calibration_step_ = 0;
    uint8_t weight_measurement_step_ = 0;

    float raw_initial_value_ = 0;
};
