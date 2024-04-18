#include "sensors_task.h"
#include "semaphore_guard.h"
#include "util.h"

// todo: think on thise compilation flags
#if SK_STRAIN
#include <HX711.h>
#endif

#if SK_ALS
#include <Adafruit_VEML7700.h>
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

    Wire.begin(PIN_SDA, PIN_SCL);
    // TODO make this configurable
    Wire.setClock(400000);

    Adafruit_VL53L0X lox = Adafruit_VL53L0X();

#if SK_STRAIN
    HX711 scale;
    scale.begin(PIN_STRAIN_DO, PIN_STRAIN_SCK);
#endif

#if SK_ALS
    Adafruit_VEML7700 veml = Adafruit_VEML7700();
    float luminosity_adjustment = 1.00;
    const float LUX_ALPHA = 0.005;
    float lux_avg;
    float lux = 0;

    if (veml.begin())
    {
        veml.setGain(VEML7700_GAIN_2);
        veml.setIntegrationTime(VEML7700_IT_400MS);
    }
    else
    {
        LOGE("Failed to boot VEML7700")
    }

#endif

    if (lox.begin())
    {
        VL53L0X_RangingMeasurementData_t measure;
        lox.rangingTest(&measure, false);
        LOGD("Proximity range %d, distance %dmm\n", measure.RangeStatus, measure.RangeMilliMeter);
    }
    else
    {
        LOGE("Failed to boot VL53L0X");
    }

    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);
    unsigned long last_proximity_check_ms = millis();
    unsigned long last_strain_check_ms = millis();
    unsigned long last_illumination_check_ms = millis();

    const uint8_t proximity_poling_rate_hz = 20;
    const uint8_t strain_poling_rate_hz = 20;
    const uint8_t illumination_poling_rate_hz = 1;

    SensorsState sensors_state = {};

    // How far button is pressed, in range [0, 1]
    float press_value_unit = 0;
    int32_t strain_reading_raw = 0;

    char buf_[128];

    // strain sensor and buttons
    unsigned long short_pressed_triggered_at_ms = 0;
    const unsigned long long_press_timeout_ms = 500;

    // strain breaking points
    const float strain_released = 0.3;
    const float strain_pressed = 1.0;

    // strain value filtering
    double strain_filtered;
    MovingAverage strain_filter(5);

    // system temperature
    long last_system_temperature_check = 0;
    float last_system_temperature = 0;

    while (1)
    {

        if (millis() - last_system_temperature_check > 1000)
        {
            temp_sensor_read_celsius(&last_system_temperature);

            sprintf(buf_, "system temp %0.2f °C", last_system_temperature);
            LOGD(buf_);

            sensors_state.system.esp32_temperature = last_system_temperature;

            last_system_temperature_check = millis();
        }

        if (millis() - last_proximity_check_ms > 1000 / proximity_poling_rate_hz)
        {

            lox.rangingTest(&measure, false);

            sensors_state.proximity.RangeMilliMeter = measure.RangeMilliMeter - PROXIMITY_SENSOR_OFFSET_MM;
            sensors_state.proximity.RangeStatus = measure.RangeStatus;
            // todo: call this once per tick
            publishState(sensors_state);
            snprintf(buf_, sizeof(buf_), "Proximity sensor:  range %d, distance %dmm\n", measure.RangeStatus, measure.RangeMilliMeter);
            LOGD(buf_);
            last_proximity_check_ms = millis();
        }
#if SK_STRAIN
        if (millis() - last_strain_check_ms > 1000 / strain_poling_rate_hz)
        {
            if (scale.wait_ready_timeout(100))
            {
                strain_reading_raw = scale.read();

                if (abs(strain_reading_raw - strain_calibration.idle_value) > abs(4 * strain_calibration.press_delta) && strain_calibration.idle_value != 0)
                {

                    snprintf(buf_, sizeof(buf_), "Value for pressure discarded. Raw Reading %d, idle value %d, delta value %d", strain_reading_raw, strain_calibration.idle_value, strain_calibration.press_delta);
                    LOGD(buf_);
                }
                else
                {
                    sensors_state.strain.raw_value = strain_filter.addSample(strain_reading_raw);
                    // TODO: calibrate and track (long term moving average) idle point (lower)
                    sensors_state.strain.press_value = lerp(sensors_state.strain.raw_value, strain_calibration.idle_value, strain_calibration.idle_value + strain_calibration.press_delta, 0, 1);

                    if (sensors_state.strain.press_value < strain_released)
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
                    else if (strain_released < sensors_state.strain.press_value && sensors_state.strain.press_value < strain_pressed)
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
                    else if (sensors_state.strain.press_value > strain_pressed)
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
                    snprintf(buf_, sizeof(buf_), "Strain: reading:  %d %d %d, [%0.2f,%0.2f] -> %0.2f ", sensors_state.strain.virtual_button_code, strain_reading_raw, sensors_state.strain.raw_value, strain_calibration.idle_value, strain_calibration.idle_value + strain_calibration.press_delta, press_value_unit);
                    LOGV(PB_LogLevel_DEBUG, buf_);
                }
            }
        }
#endif

#if SK_ALS
        if (millis() - last_illumination_check_ms > 1000 / illumination_poling_rate_hz)
        {

            lux = veml.readLux();
            lux_avg = lux * LUX_ALPHA + lux_avg * (1 - LUX_ALPHA);

            // looks at the lower part of the sensor spectrum (0 = dark)

            luminosity_adjustment = min(1.0f, lux_avg);

            sensors_state.illumination.lux = lux;
            sensors_state.illumination.lux_avg = lux_avg;
            sensors_state.illumination.lux_adj = luminosity_adjustment;

            snprintf(buf_, sizeof(buf_), "Illumination sensor: millilux: %.2f, avg %.2f, adj %.2f", lux * 1000, lux_avg * 1000, luminosity_adjustment);
            LOGV(PB_LogLevel_DEBUG, buf_);
            last_illumination_check_ms = millis();
        }
#endif
        delay(1);
    }
}

void SensorsTask::updateStrainCalibration(float idle_value, float press_delta)
{
    strain_calibration.idle_value = idle_value;
    strain_calibration.press_delta = press_delta;
    char buf_[128];
    snprintf(buf_, sizeof(buf_), "New strain config, idle: %d, pressed: %d ", strain_calibration.idle_value, strain_calibration.idle_value + strain_calibration.press_delta);

    LOGI(buf_);
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
