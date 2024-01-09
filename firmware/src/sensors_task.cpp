#if SK_SEEDLABS_DEVKIT
#include "sensors_task.h"
#include "semaphore_guard.h"
#include "util.h"

SensorsTask::SensorsTask(const uint8_t task_core) : Task{"Sensors", 2048 * 2, 1, task_core}
{
    mutex_ = xSemaphoreCreateMutex();

    sensors_state_queue_ = xQueueCreate(50, sizeof(SensorsState));
    assert(sensors_state_queue_ != NULL);
    assert(mutex_ != NULL);
}

SensorsTask::~SensorsTask()
{
    vQueueDelete(sensors_state_queue_);

    vSemaphoreDelete(mutex_);
}

void SensorsTask::run()
{
    Adafruit_VL53L0X lox = Adafruit_VL53L0X();

    if (!lox.begin())
    {
        log("Failed to boot VL53L0X");
    }
    else
    {
        VL53L0X_RangingMeasurementData_t measure;
        lox.rangingTest(&measure, false);
        ESP_LOGD("app_task.cpp", "Proximity range %d, distance %dmm\n", measure.RangeStatus, measure.RangeMilliMeter);
    }

    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);
    unsigned long last_proximity_check_ms = millis();

    SensorsState sensors_state = {};

    while (1)
    {
        if (millis() - last_proximity_check_ms > 100)
        {

            lox.rangingTest(&measure, false);

            last_proximity_check_ms = millis();
            sensors_state.proximity.RangeMilliMeter = measure.RangeMilliMeter - PROXIMITY_SENSOR_OFFSET_MM;
            sensors_state.proximity.RangeStatus = measure.RangeStatus;
            publishState(sensors_state);
            // ESP_LOGD("app_task.cpp", "Proximity range %d, distance %dmm\n", measure.RangeStatus, measure.RangeMilliMeter);
        }
        delay(5);
    }
}

void SensorsTask::addStateListener(QueueHandle_t queue)
{
    state_listeners_.push_back(queue);
}

void SensorsTask::publishState(const SensorsState &state)
{
    for (auto listener : state_listeners_)
    {
        xQueueSend(listener, &state, portMAX_DELAY);

        // xQueueOverwrite(listener, &state);
    }
}

void SensorsTask::setLogger(Logger *logger)
{
    logger_ = logger;
}

void SensorsTask::log(const char *msg)
{
    if (logger_ != nullptr)
    {
        logger_->log(msg);
    }
}

#endif