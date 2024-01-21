#if SK_SEEDLABS_DEVKIT
#include "sensors_task.h"
#include "semaphore_guard.h"
#include "util.h"

// todo: think on thise compilation flags
#if SK_STRAIN
#include <HX711.h>
#endif

static const char *TAG = "sensors_task";

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

#if SK_STRAIN
    HX711 scale;
    scale.begin(PIN_STRAIN_DO, PIN_STRAIN_SCK);
#endif

    if (!lox.begin())
    {
        log("Failed to boot VL53L0X");
    }
    else
    {
        VL53L0X_RangingMeasurementData_t measure;
        lox.rangingTest(&measure, false);
        ESP_LOGD(TAG, "Proximity range %d, distance %dmm\n", measure.RangeStatus, measure.RangeMilliMeter);
    }

    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);
    unsigned long last_proximity_check_ms = millis();
    unsigned long last_strain_check_ms = millis();

    const uint8_t proximity_poling_rate_hz = 20;
    const uint8_t strain_poling_rate_hz = 20;

    SensorsState sensors_state = {};

    // How far button is pressed, in range [0, 1]
    float press_value_unit = 0;
    int32_t strain_reading_ = 0;

    char buf_[128];

    // strain sensor and buttons
    unsigned long short_pressed_triggered_at_ms = 0;
    const unsigned long long_press_timeout_ms = 1000;

    while (1)
    {
        if (millis() - last_proximity_check_ms > 1000 / proximity_poling_rate_hz)
        {

            lox.rangingTest(&measure, false);

            last_proximity_check_ms = millis();
            sensors_state.proximity.RangeMilliMeter = measure.RangeMilliMeter - PROXIMITY_SENSOR_OFFSET_MM;
            sensors_state.proximity.RangeStatus = measure.RangeStatus;
            // todo: call this once per tick
            publishState(sensors_state);

            if (verbose_)
            {
                snprintf(buf_, sizeof(buf_), "Proximity range %d, distance %dmm\n", measure.RangeStatus, measure.RangeMilliMeter);
                log(buf_);
            }
        }

        if (millis() - last_strain_check_ms > 1000 / strain_poling_rate_hz)
        {
            if (scale.wait_ready_timeout(100))
            {
                strain_reading_ = scale.read();
                sensors_state.strain.raw_value = strain_reading_;
                // TODO: calibrate and track (long term moving average) idle point (lower)
                press_value_unit = lerp(strain_reading_, strain_calibration.idle_value, strain_calibration.idle_value + strain_calibration.press_delta, 0, 1);
                sensors_state.strain.press_value = press_value_unit;

                if (press_value_unit < 0.5)
                {
                    // released
                    switch (sensors_state.strain.virtual_button_code)
                    {
                    case VIRTUAL_BUTTON_SHORT_PRESSED:
                        short_pressed_triggered_at_ms = 0;
                        sensors_state.strain.virtual_button_code = VIRTUAL_BUTTON_SHORT_RELEASED;
                        break;
                    case VIRTUAL_BUTTON_LONG_PRESSED:
                        short_pressed_triggered_at_ms = 0;
                        sensors_state.strain.virtual_button_code = VIRTUAL_BUTTON_LONG_RELEASED;
                        break;
                    default:
                        short_pressed_triggered_at_ms = 0;
                        sensors_state.strain.virtual_button_code = VIRTUAL_BUTTON_IDLE;
                        break;
                    }
                }
                else if (0.5 < press_value_unit && press_value_unit < 1)
                {
                    switch (sensors_state.strain.virtual_button_code)
                    {

                    case VIRTUAL_BUTTON_SHORT_PRESSED:
                        if (short_pressed_triggered_at_ms > 0 && millis() - short_pressed_triggered_at_ms > long_press_timeout_ms)
                        {
                            sensors_state.strain.virtual_button_code = VIRTUAL_BUTTON_LONG_PRESSED;
                        }
                        break;

                    default:
                        break;
                    }
                }
                else if (press_value_unit > 1)
                {
                    switch (sensors_state.strain.virtual_button_code)
                    {
                    case VIRTUAL_BUTTON_IDLE:
                        sensors_state.strain.virtual_button_code = VIRTUAL_BUTTON_SHORT_PRESSED;
                        short_pressed_triggered_at_ms = millis();
                        break;

                    case VIRTUAL_BUTTON_SHORT_PRESSED:
                        if (short_pressed_triggered_at_ms > 0 && millis() - short_pressed_triggered_at_ms > long_press_timeout_ms)
                        {
                            sensors_state.strain.virtual_button_code = VIRTUAL_BUTTON_LONG_PRESSED;
                        }
                        break;

                    default:
                        break;
                    }
                }

                // todo: call this once per tick
                publishState(sensors_state);
                if (verbose_)
                {
                    snprintf(buf_, sizeof(buf_), "Strain reading:  %d %d, [%0.2f,%0.2f] -> %0.2f ", sensors_state.strain.virtual_button_code, strain_reading_, strain_calibration.idle_value, strain_calibration.idle_value + strain_calibration.press_delta, press_value_unit);
                    log(buf_);
                }
            }
        }

        delay(1);
    }
}

void SensorsTask::updateStrainCalibration(float idle_value, float press_delta)
{
    strain_calibration.idle_value = idle_value;
    strain_calibration.press_delta = press_delta;
    char buf_[128];
    snprintf(buf_, sizeof(buf_), "New strain config, idle: %d, pressed: %d ", strain_calibration.idle_value, strain_calibration.idle_value + strain_calibration.press_delta);

    log(buf_);
}

void SensorsTask::addStateListener(QueueHandle_t queue)
{
    state_listeners_.push_back(queue);
}

void SensorsTask::setVerbose(bool verbose)
{
    verbose_ = verbose;
}

void SensorsTask::toggleVerbose()
{
    verbose_ = !verbose_;
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