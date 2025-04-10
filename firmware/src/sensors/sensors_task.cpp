#include "sensors_task.h"
#include "semaphore_guard.h"
#include "util.h"

// todo: think on thise compilation flags

#if SK_ALS
#include <Adafruit_VEML7700.h>
#endif

static const char *TAG = "sensors_task";

SensorsTask::SensorsTask(const uint8_t task_core, Configuration *configuration) : Task{"Sensors", 1024 * 8, 0, task_core}, configuration_(configuration)
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
    strain.begin(PIN_STRAIN_DO, PIN_STRAIN_SCK);
    while (!strain.is_ready())
    {
        LOGV(LOG_LEVEL_DEBUG, "Strain sensor not ready, waiting...");
        delay(100);
    }
    if (configuration_->get().strain_scale == 0)
    {
        calibration_scale_ = 1.0f;
    }
    else
    {
        calibration_scale_ = configuration_->get().strain_scale;
    }
    LOGV(LOG_LEVEL_DEBUG, "Strain scale set at boot, %f", calibration_scale_);
    strain.set_scale(calibration_scale_);
    delay(100);
    strain.set_offset(0);
    strain.tare();
    delay(100);

    strain_powered = true;
    raw_initial_value_ = strain.get_units(10);
#endif

#if SK_ALS
    Adafruit_VEML7700 veml = Adafruit_VEML7700();
    float luminosity_adjustment = 1.00;
    const float LUX_ALPHA = 0.005;

    float sum = 0.0;
    float lux_avg;
    float lux = 0.0;

    if (veml.begin())
    {
        veml.setGain(VEML7700_GAIN_2);
        veml.setIntegrationTime(VEML7700_IT_400MS);
    }
    else
    {
        LOGE("Failed to boot VEML7700");
    }

    delay(1000); // Wait for VEML7700 to boot 500ms seems to not be enough...
    lux = veml.readLux();
    MovingAverage lux_filter(10);
    for (uint8_t i = 0; i < 10; i++)
    {
        lux_filter.addSample(lux);
    }
#endif
    if (lox.begin())
    {
        VL53L0X_RangingMeasurementData_t measure;
        lox.rangingTest(&measure, false);
        LOGV(LOG_LEVEL_DEBUG, "Proximity range %d, distance %dmm", measure.RangeStatus, measure.RangeMilliMeter);
    }
    else
    {
        LOGE("Failed to boot VL53L0X");
    }

    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);
    unsigned long last_proximity_check_ms = 0;
    unsigned long last_strain_check_ms = 0;
    unsigned long last_tare_ms = 0;
    unsigned long last_illumination_check_ms = 0;

    unsigned long log_ms_calib = 10000;

    unsigned long log_ms = 0;
    unsigned long log_ms_strain = 0;

    const uint8_t proximity_poling_rate_hz = 20;
    const uint8_t strain_poling_rate_hz = 120;
    const uint8_t illumination_poling_rate_hz = 1;

    // How far button is pressed, in range [0, 1]
    float press_value_unit = 0;
    float strain_reading_raw = 0;

    char buf_[128];

    // strain sensor and buttons
    unsigned long short_pressed_triggered_at_ms = 0;
    const unsigned long long_press_timeout_ms = 500;

    // strain breaking points
    const float strain_released = 0.3;
    const float strain_pressed = 1.0;

    // strain value filtering
    double strain_filtered;
    MovingAverage strain_filter(10);

    // system temperature
    long last_system_temperature_check = 0;
    float last_system_temperature = 0;

    uint8_t discarded_strain_reading_count = 0;

    // Add a predefined value for the minimum raw strain reading
    const float MIN_RAW_STRAIN_READING = 1000.0;

    while (1)
    {
        if (millis() - last_system_temperature_check > 1000)
        {
            temp_sensor_read_celsius(&last_system_temperature);

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
            last_proximity_check_ms = millis();
        }
#if SK_STRAIN
        if (millis() - last_strain_check_ms > 1000 / strain_poling_rate_hz)
        {
            if (strain_powered && strain.wait_ready_timeout(100))
            {
                if (calibration_scale_ == 1.0f && strain.get_scale() == 1.0f && factory_strain_calibration_step_ == 0)
                {
                    if (millis() - log_ms_calib > 10000)
                    {
                        LOGW("Strain sensor needs Factory Calibration, press 'Y' to begin!");
                        log_ms_calib = millis();
                    }
                    do_strain = false;
                }
                else if (weight_measurement_step_ != 0 || factory_strain_calibration_step_ != 0)
                {
                    delay(1);
                    do_strain = false;
                }

                if (do_strain)
                {

                    strain_reading_raw = strain.get_units(1);

                    if (abs(last_strain_reading_raw_ - strain_reading_raw) > 2000)
                    {
                        discarded_strain_reading_count++;
                        if (discarded_strain_reading_count > 20)
                        {
                            LOGV(LOG_LEVEL_WARNING, "Resetting strain sensor. 20 consecutive readings discarded.");
                            strain.power_down();
                            delay(100);
                            strain.power_up();
                            delay(100);
                            strain.set_offset(0);
                            strain.tare();
                            delay(100);
                            discarded_strain_reading_count = 0;
                        }

                        LOGW("Discarding strain reading, too big difference from last reading.");
                        LOGV(LOG_LEVEL_WARNING, "Current raw strain reading: %f", strain_reading_raw);
                        LOGV(LOG_LEVEL_WARNING, "Last raw strain reading: %f", last_strain_reading_raw_);
                    }
                    else
                    {
                        discarded_strain_reading_count = 0;
                        sensors_state.strain.raw_value = strain_filter.addSample(strain_reading_raw);

                        // LOGD("Strain raw reading: %f", sensors_state.strain.raw_value);

                        // TODO: calibrate and track (long term moving average) idle point (lower)
                        sensors_state.strain.press_value = lerp(sensors_state.strain.raw_value, 0, PRESS_WEIGHT, 0, 1);

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
                                delay(20);
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
                                LOGV(LOG_LEVEL_DEBUG, "Strain sensor short press.");
                                LOGV(LOG_LEVEL_DEBUG, "Press value: %f", sensors_state.strain.press_value);
                                LOGV(LOG_LEVEL_DEBUG, "Raw value: %f", sensors_state.strain.raw_value);
                                LOGV(LOG_LEVEL_DEBUG, "Last press value: %f", last_press_value_);
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

                        publishState(sensors_state);

                        if (sensors_state.strain.virtual_button_code == VIRTUAL_BUTTON_IDLE && millis() - short_pressed_triggered_at_ms > 100 && press_value_unit < strain_released && 0.025 < abs(sensors_state.strain.press_value - last_press_value_) < 0.1 && millis() - last_tare_ms > 10000)
                        {
                            LOGV(LOG_LEVEL_DEBUG, "Strain sensor tare.");
                            strain.tare();
                            last_tare_ms = millis();
                        }

                        last_strain_reading_raw_ = strain_reading_raw;
                        last_press_value_ = sensors_state.strain.press_value;
                        last_strain_check_ms = millis();
                    }

                    // Add logic to check if the raw strain reading is lower than the predefined value
                    if (strain_reading_raw < MIN_RAW_STRAIN_READING)
                    {
                        // Trigger a user prompt if the raw strain reading is lower than the predefined value
                        WiFiEvent event;
                        event.type = SK_STRAIN_SENSOR_ERROR;
                        publishEvent(event);
                    }
                }

                do_strain = true;
            }
            else
            {
                if (do_strain && strain_powered && millis() - log_ms_strain > 4000)
                {
                    LOGV(LOG_LEVEL_DEBUG, "Strain sensor not ready, waiting...");
                    log_ms_strain = millis();
                }
                else if (millis() - log_ms_strain > 4000)
                {
                    LOGV(LOG_LEVEL_DEBUG, "Strain sensor is disabled. (Might be because of factory calib or its powered off because no engagement of knob)");
                    log_ms_strain = millis();
                }
            }
        }
#endif

#if SK_ALS
        if (millis() - last_illumination_check_ms > 1000 / illumination_poling_rate_hz)
        {

            lux = veml.readLux();

            lux_avg = lux_filter.addSample(lux);

            luminosity_adjustment = min(1.0f, lux_avg);

            sensors_state.illumination.lux = lux;
            sensors_state.illumination.lux_avg = lux_avg;
            sensors_state.illumination.lux_adj = luminosity_adjustment;

            last_illumination_check_ms = millis();
        }
#endif

        if (millis() - log_ms > 1000)
        {
            LOGV(LOG_LEVEL_DEBUG, "System temp %0.2f °C", last_system_temperature);
            LOGV(LOG_LEVEL_DEBUG, "Proximity sensor:  range %d, distance %dmm", measure.RangeStatus, measure.RangeMilliMeter);
#if SK_STRAIN
            LOGV(LOG_LEVEL_DEBUG, "Strain: reading:\n        Virtual button code: %d\n        Strain value: %f\n        Press value: %f", sensors_state.strain.virtual_button_code, sensors_state.strain.raw_value, press_value_unit);
#endif
#if SK_ALS
            LOGV(LOG_LEVEL_DEBUG, "Illumination sensor: millilux: %.2f, avg %.2f, adj %.2f", lux * 1000, lux_avg * 1000, luminosity_adjustment);
#endif
            log_ms = millis();
        }
        delay(1);
    }
}

#if SK_STRAIN
void SensorsTask::factoryStrainCalibrationCallback(float calibration_weight)
{
    WiFiEvent event;
    event.type = SK_STRAIN_CALIBRATION;
    event.body.calibration_step = factory_strain_calibration_step_;
    publishEvent(event);
    if (factory_strain_calibration_step_ == 0)
    {
        factory_strain_calibration_step_ = 1;
        LOGI("Factory strain calibration step 1");

        strainPowerUp();

        delay(200);

        strain.set_scale();
        delay(200);

        strain.set_offset(0);
        strain.tare();
        delay(200);

        raw_initial_value_ = strain.get_units(10);

        LOGI("Place calibration weight on the knob and press 'Y' again");

        delay(2000);
        return;
    }

    // Array of calibration floats
    float calibration_scale_validation[3];

    LOGI("Factory strain calibration step 2, try: %d", factory_strain_calibration_step_);
    float raw_value = strain.get_units(10);

    LOGD("Raw value: %0.2f", raw_value);
    LOGD("Raw initial value: %0.2f", raw_initial_value_);

    if (abs(abs(raw_initial_value_) - abs(raw_value)) < 10000)
    {
        LOGE("Calibration weight not detected. Please place the calibration weight on the knob and press 'Y' again");
        if (configuration_->get().strain_scale == 0)
        {
            calibration_scale_ = 1.0f;
        }
        else
        {
            calibration_scale_ = configuration_->get().strain_scale;
        }
        LOGV(LOG_LEVEL_DEBUG, "Strain scale set at boot, %f", calibration_scale_);
        strain.set_scale(calibration_scale_);
        delay(100);
        strain.set_offset(0);
        strain.tare();
        delay(100);

        factory_strain_calibration_step_ = 0;
        return;
    }

    calibration_scale_ = 0;

    for (size_t i = 0; i < 3; i++)
    {
        strain.set_scale();
        delay(100);
        raw_value = strain.get_units(10);
        LOGD("Raw value during calibration: %0.2f", raw_value);
        calibration_scale_ = raw_value / calibration_weight;

        strain.set_scale(calibration_scale_);
        delay(200);
        float calibrated_weight = strain.get_units(10);

        while (abs(calibrated_weight - calibration_weight) > 0.25)
        {
            if (abs(calibrated_weight - calibration_weight) > 10)
            {
                LOGE("Calibrated weight is more than 10g off from the calibration weight. Restart calibration by pressing 'Y' again.");
                delay(2000);
                strain.set_scale(1.0f);
                calibration_scale_ = 1.0f;
                factory_strain_calibration_step_ = 0;
                return;
            }

            if (calibrated_weight < calibration_weight)
            {
                calibration_scale_ -= abs((calibrated_weight - calibration_weight));
            }
            else
            {
                calibration_scale_ += abs((calibrated_weight - calibration_weight));
            }

            strain.set_scale(calibration_scale_);
            delay(200);
            calibrated_weight = strain.get_units(10);
            LOGD("Measured weight during calibration: %0.2fg", calibrated_weight); // MAKE VERBOSE LATER
        }
        LOGD("Validation run %d, result: %0.2fg", i + 1, calibrated_weight);
        calibration_scale_validation[i] = calibration_scale_;
    }

    strain.set_scale((calibration_scale_validation[0] + calibration_scale_validation[1] + calibration_scale_validation[2]) / 3.0f);

    configuration_->saveFactoryStrainCalibration((calibration_scale_validation[0] + calibration_scale_validation[1] + calibration_scale_validation[2]) / 3.0f);

    LOGI("Found calibration scale: %f", (calibration_scale_validation[0] + calibration_scale_validation[1] + calibration_scale_validation[2]) / 3.0f);
    LOGD("Found strain scale. Verifying...");
    for (size_t i = 0; i < 3; i++)
    {
        delay(1000);
        LOGD("Verify calibrated weight: %0.0fg", strain.get_units(10));
    }
    LOGI("\nRemove calibration weight.\n");
    delay(8000);
    LOGI("Factory strain calibration complete!");
    strain.set_offset(0);
    strain.tare();
    factory_strain_calibration_step_ = 0;
}

void SensorsTask::weightMeasurementCallback()
{
    if (weight_measurement_step_ == 0)
    {
        weight_measurement_step_ = 1;
        LOGI("Weight measurement step 1: Place weight on KNOB and press 'w' again");
        delay(1000);
        strain.set_offset(0);
        strain.tare();
    }
    else if (weight_measurement_step_ == 1)
    {
        LOGD("Measured weight: %0.0fg", strain.get_units(10));
        weight_measurement_step_ = 0;
    }
}

bool SensorsTask::powerDownAllowed()
{
    // If strain sensor isnt calibrated dont allow power down.
    if (calibration_scale_ == 1.0f && strain.get_scale() == 1.0f)
    {
        return false;
    }

    // If calibration or weight measurement is in progress, dont allow power down.
    if (weight_measurement_step_ != 0 || factory_strain_calibration_step_ != 0)
    {
        return false;
    }

    // Allow power down.
    return true;
}

void SensorsTask::strainPowerDown()
{
    if (!powerDownAllowed())
    {
        return;
    }

    if (strain.wait_ready_timeout(10)) // Make sure sensor is on before powering down.
    {
        LOGV(LOG_LEVEL_DEBUG, "Strain sensor power down.");

        strain_powered = false;
        strain.power_down();
    }
}

void SensorsTask::strainPowerUp() // Delays caused a perceived delay in the activation of strain.
{
    if (!strain.wait_ready_timeout(10)) // Make sure sensor is off before powering up.
    {
        LOGV(LOG_LEVEL_DEBUG, "Strain sensor power up.");

        strain.power_up();
        if (strain.wait_ready_timeout(500))
        {
            strain.set_offset(0);
            strain.tare();
            last_strain_reading_raw_ = strain.get_units(10);
            strain_powered = true;
        }
        else
        {
            LOGE("Strain sensor not ready after power up!!!");
        }
    }
}
#endif

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

void SensorsTask::setSharedEventsQueue(QueueHandle_t shared_events_queue)
{
    this->shared_events_queue = shared_events_queue;
}

void SensorsTask::publishEvent(WiFiEvent event)
{
    event.sent_at = millis();
    xQueueSendToBack(shared_events_queue, &event, 0);
}
