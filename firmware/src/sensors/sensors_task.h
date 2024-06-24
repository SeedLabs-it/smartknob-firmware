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

class SensorsTask : public Task<SensorsTask>
{
    friend class Task<SensorsTask>; // Allow base Task to invoke protected run()

public:
    SensorsTask(const uint8_t task_core, Configuration *configuration);
    ~SensorsTask();

    void addStateListener(QueueHandle_t queue);
    void factoryStrainCalibrationCallback(float calibration_weight);
    void weightMeasurementCallback();

    void setSharedEventsQueue(QueueHandle_t shared_event_queue);
    void publishEvent(WiFiEvent event);

    void strainPowerDown();
    void strainPowerUp();

protected:
    void run();

private:
    SensorsState sensors_state = {};
    QueueHandle_t sensors_state_queue_;

    bool do_strain = false;
    bool strain_powered = false;

    QueueHandle_t shared_events_queue;

    std::vector<QueueHandle_t> state_listeners_;

    SemaphoreHandle_t mutex_;
    void publishState(const SensorsState &state);
#if SK_STRAIN
    HX711 strain;
#endif

    Configuration *configuration_;

    uint8_t factory_strain_calibration_step_ = 0;
    uint8_t weight_measurement_step_ = 0;

    float last_press_value_ = 0;
    float last_strain_reading_raw_ = 0;

    float raw_initial_value_ = 0;

    float calibration_scale_ = 0;
};
