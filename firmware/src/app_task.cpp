#if SK_STRAIN
#include <HX711.h>
#endif

#if SK_ALS
#include <Adafruit_VEML7700.h>
#endif

#include "app_task.h"
#include "semaphore_guard.h"
#include "util.h"

#if SK_STRAIN
HX711 scale;
#endif

#if SK_ALS
Adafruit_VEML7700 veml = Adafruit_VEML7700();
#endif

AppTask::AppTask(
    const uint8_t task_core,
    MotorTask &motor_task,
    DisplayTask *display_task,
    NetworkingTask *networking_task,
    LedRingTask *led_ring_task) : Task("App", 4800, 1, task_core),
                                  stream_(),
                                  motor_task_(motor_task),
                                  display_task_(display_task),
                                  networking_task_(networking_task),
                                  led_ring_task_(led_ring_task),
                                  plaintext_protocol_(stream_, [this]()
                                                      { motor_task_.runCalibration(); }),
                                  proto_protocol_(stream_, [this](PB_SmartKnobConfig &config)
                                                  { applyConfig(config, true); })
{
#if SK_DISPLAY
    assert(display_task != nullptr);
#endif

    app_sync_queue_ = xQueueCreate(2, sizeof(cJSON *));
    assert(app_sync_queue_ != NULL);

    log_queue_ = xQueueCreate(10, sizeof(std::string *));
    assert(log_queue_ != NULL);

    knob_state_queue_ = xQueueCreate(1, sizeof(PB_SmartKnobState));
    assert(knob_state_queue_ != NULL);

    connectivity_status_queue_ = xQueueCreate(1, sizeof(ConnectivityState));
    assert(connectivity_status_queue_ != NULL);

    sensors_status_queue_ = xQueueCreate(100, sizeof(SensorsState));
    assert(sensors_status_queue_ != NULL);

    mutex_ = xSemaphoreCreateMutex();
    assert(mutex_ != NULL);
}

AppTask::~AppTask()
{
    vSemaphoreDelete(mutex_);
}

void AppTask::setApps(Apps *apps)
{
    this->apps = apps;
}

void AppTask::run()
{
    stream_.begin();

#if SK_ALS && PIN_SDA >= 0 && PIN_SCL >= 0
    Wire.begin(PIN_SDA, PIN_SCL);
    Wire.setClock(400000);
#endif
#if SK_STRAIN
    scale.begin(PIN_STRAIN_DO, PIN_STRAIN_SCK);
#endif

#if SK_ALS
    if (veml.begin())
    {
        veml.setGain(VEML7700_GAIN_2);
        veml.setIntegrationTime(VEML7700_IT_400MS);
    }
    else
    {
        log("ALS sensor not found!");
    }
#endif

    log("Giving 0.5s for Apps to initialize");
    delay(500);

    apps->setActive(menu_type, 0);
    applyConfig(apps->getActiveMotorConfig(), false);
    motor_task_.addListener(knob_state_queue_);

    plaintext_protocol_.init([this]()
                             { changeConfig(std::make_pair(menu_type, 0)); },
                             [this]()
                             {
                                 if (!configuration_loaded_)
                                 {
                                     return;
                                 }
                                 if (strain_calibration_step_ == 0)
                                 {
                                     log("Strain calibration step 1: Don't touch the knob, then press 'S' again");
                                     strain_calibration_step_ = 1;
                                 }
                                 else if (strain_calibration_step_ == 1)
                                 {
                                     configuration_value_.strain.idle_value = strain_reading_;
                                     snprintf(buf_, sizeof(buf_), "  idle_value=%d", configuration_value_.strain.idle_value);
                                     log(buf_);
                                     log("Strain calibration step 2: Push and hold down the knob with medium pressure, and press 'S' again");
                                     strain_calibration_step_ = 2;
                                 }
                                 else if (strain_calibration_step_ == 2)
                                 {

                                     configuration_value_.strain.press_delta = strain_reading_ - configuration_value_.strain.idle_value;
                                     configuration_value_.has_strain = true;

                                     ESP_LOGD("1", "pre-save %d", configuration_value_.strain.idle_value);
                                     snprintf(buf_, sizeof(buf_), "  press_delta=%d", configuration_value_.strain.press_delta);
                                     log(buf_);
                                     log("Strain calibration complete! Saving...");

                                     ESP_LOGD("2", "pre-save %d", configuration_value_.strain.idle_value);
                                     strain_calibration_step_ = 0;
                                     if (configuration_->setStrainCalibrationAndSave(configuration_value_.strain))
                                     {
                                         log("  Saved!");
                                     }
                                     else
                                     {
                                         log("  FAILED to save config!!!");
                                     }
                                 }
                             });

    // Start in legacy protocol mode
    current_protocol_ = &plaintext_protocol_;

    ProtocolChangeCallback protocol_change_callback = [this](uint8_t protocol)
    {
        switch (protocol)
        {
        case SERIAL_PROTOCOL_LEGACY:
            current_protocol_ = &plaintext_protocol_;
            break;
        case SERIAL_PROTOCOL_PROTO:
            current_protocol_ = &proto_protocol_;
            break;
        default:
            log("Unknown protocol requested");
            break;
        }
    };

    plaintext_protocol_.setProtocolChangeCallback(protocol_change_callback);
    proto_protocol_.setProtocolChangeCallback(protocol_change_callback);

    EntityStateUpdate entity_state_update_to_send;

    // Value between [0, 65536] for brightness when not engaging with knob
    bool isCurrentSubPositionSet = false;
    float currentSubPosition;
    AppState app_state = {};
    while (1)
    {
        if (xQueueReceive(sensors_status_queue_, &latest_sensors_state_, 0) == pdTRUE)
        {
            app_state.proximiti_state.RangeMilliMeter = latest_sensors_state_.proximity.RangeMilliMeter;
            app_state.proximiti_state.RangeStatus = latest_sensors_state_.proximity.RangeStatus;

            // wake up the screen
            // RangeStatus is usually 0,2,4. We want to caputure the level of confidence 0 and 2.
            if (app_state.proximiti_state.RangeStatus < 3 && app_state.proximiti_state.RangeMilliMeter < 200)
            {
                app_state.screen_state.has_been_engaged = true;
            }
        }
        if (app_state.screen_state.has_been_engaged == true)
        {
            app_state.screen_state.brightness = app_state.screen_state.MAX_LCD_BRIGHTNESS;
            app_state.screen_state.awake_until = millis() + 1000; // 1s
        }
        // Check if the knob is awake, and if the time is expired
        // and set it to not engaged
        if (app_state.screen_state.has_been_engaged && millis() < app_state.screen_state.awake_until)
        {
            app_state.screen_state.has_been_engaged = false;
        }
        if (app_state.screen_state.has_been_engaged == false &&
            app_state.screen_state.brightness > app_state.screen_state.MIN_LCD_BRIGHTNESS &&
            millis() > app_state.screen_state.awake_until)
        {
            // TODO, this can be timed better (ideally by subtracting MAX_BRIGHTNESS - MIN_BRIGHTNESS/fps/second_of_animation)
            app_state.screen_state.brightness = app_state.screen_state.brightness - 1000;
            if (app_state.screen_state.brightness < app_state.screen_state.MIN_LCD_BRIGHTNESS)
                app_state.screen_state.brightness = app_state.screen_state.MIN_LCD_BRIGHTNESS;
        }

        if (xQueueReceive(connectivity_status_queue_, &latest_connectivity_state_, 0) == pdTRUE)
        {
            app_state.connectivity_state = latest_connectivity_state_;
        }

        if (xQueueReceive(app_sync_queue_, &apps_, 0) == pdTRUE)
        {
            ESP_LOGD("app_task", "App sync requested!");
#if SK_NETWORKING // Should this be here??
            apps->reload(networking_task_->getApps());

            // SHOULD BE RELEASE LATER WHEN RELOAD IS DONE
            networking_task_->unlock();
#endif

            // cJSON_Delete(apps_);
        }
        if (xQueueReceive(knob_state_queue_, &latest_state_, 0) == pdTRUE)
        {

            // The following is a smoothing filter (rounding) on the sub position unit (to avoid flakines).
            float roundedNewPosition = round(latest_state_.sub_position_unit * 3) / 3.0;
            // This if is used to understand if we have touched the knob since last state.
            if (isCurrentSubPositionSet)
            {
                if (currentSubPosition != roundedNewPosition)
                {
                    // We set a flag on the object Screen State.
                    //  Todo: this property should be at app state and not screen state
                    app_state.screen_state.has_been_engaged = true;
                }
            }
            isCurrentSubPositionSet = true;
            currentSubPosition = roundedNewPosition;
            app_state.motor_state = latest_state_;

            entity_state_update_to_send = apps->update(app_state);

#if SK_NETWORKING
            networking_task_->enqueueEntityStateToSend(entity_state_update_to_send);
#endif

            if (entity_state_update_to_send.play_haptic)
            {
                motor_task_.playHaptic(true);
            }

            publish(app_state);
            publishState();
        }

        current_protocol_->loop();

        std::string *log_string;
        while (xQueueReceive(log_queue_, &log_string, 0) == pdTRUE)
        {
            current_protocol_->log(log_string->c_str());
            delete log_string;
        }

        updateHardware(app_state);

        if (!configuration_loaded_)
        {
            SemaphoreGuard lock(mutex_);
            if (configuration_ != nullptr)
            {
                configuration_value_ = configuration_->get();
                configuration_loaded_ = true;
            }
        }

        delay(1);
    }
}

void AppTask::log(const char *msg)
{
    // Allocate a string for the duration it's in the queue; it is free'd by the queue consumer
    std::string *msg_str = new std::string(msg);

    // Put string in queue (or drop if full to avoid blocking)
    xQueueSendToBack(log_queue_, &msg_str, 0);
}

void AppTask::changeConfig(std::pair<app_types, uint8_t> next)
{
    apps->setActive(next.first, next.second); // TODO LOOK OVER

    applyConfig(apps->getActiveMotorConfig(), false);
}

void AppTask::updateHardware(AppState app_state)
{
    // How far button is pressed, in range [0, 1]
    float press_value_unit = 0;

#if SK_ALS
    const float LUX_ALPHA = 0.005;
    static float lux_avg;
    float lux = veml.readLux();
    lux_avg = lux * LUX_ALPHA + lux_avg * (1 - LUX_ALPHA);
    static uint32_t last_als;
    if (millis() - last_als > 1000 && strain_calibration_step_ == 0)
    {
        snprintf(buf_, sizeof(buf_), "millilux: %.2f", lux * 1000);
        log(buf_);

        last_als = millis();
    }
#endif

    static bool pressed;
#if SK_STRAIN
    if (scale.wait_ready_timeout(100))
    {
        strain_reading_ = scale.read();

        static uint32_t last_reading_display;
        if (millis() - last_reading_display > 1000 && strain_calibration_step_ == 0)
        {
            snprintf(buf_, sizeof(buf_), "HX711 reading: %d", strain_reading_);
            log(buf_);
            last_reading_display = millis();
        }
        if (configuration_loaded_ && configuration_value_.has_strain && strain_calibration_step_ == 0)
        {
            // TODO: calibrate and track (long term moving average) idle point (lower)
            press_value_unit = lerp(strain_reading_, configuration_value_.strain.idle_value, configuration_value_.strain.idle_value + configuration_value_.strain.press_delta, 0, 1);

            // Ignore readings that are way out of expected bounds
            if (-1 < press_value_unit && press_value_unit < 2)
            {
                static uint8_t press_readings;
                if (!pressed && press_value_unit > 1)
                {
                    press_readings++;
                    if (press_readings > 2)
                    {
                        motor_task_.playHaptic(true);
                        pressed = true;
                        press_count_++;
                        publishState();
                        if (!remote_controlled_)
                        {
                            changeConfig(apps->navigationNext());
                        }
                    }
                }
                else if (pressed && press_value_unit < 0.5)
                {
                    press_readings++;
                    if (press_readings > 2)
                    {
                        motor_task_.playHaptic(false);
                        pressed = false;
                    }
                }
                else
                {
                    press_readings = 0;
                }
            }
        }
    }
    else
    {
        log("HX711 not found.");

        // #if SK_LEDS
        //         for (uint8_t i = 0; i < NUM_LEDS; i++)
        //         {
        //             leds[i] = CRGB::Red;
        //         }
        //         FastLED.show();
        // #endif
    }
#endif

    uint16_t brightness = UINT16_MAX;
// TODO: brightness scale factor should be configurable (depends on reflectivity of surface)
#if SK_ALS
    brightness = (uint16_t)CLAMP(lux_avg * 13000, (float)1280, (float)UINT16_MAX);

    brightness = app_state.screen_state.brightness;
#endif

#if SK_DISPLAY
    display_task_->setBrightness(brightness); // TODO: apply gamma correction
#endif

    if (led_ring_task_ != nullptr)
    {

        // ESP_LOGD("app_task", "%d", brightness);
        EffectSettings effect_settings;

        if (brightness < 2000)
        {

            effect_settings.effect_id = 2;
            effect_settings.effect_start_pixel = 0;
            effect_settings.effect_end_pixel = NUM_LEDS;
            effect_settings.effect_accent_pixel = 0;
            effect_settings.effect_accent_color = (255 << 16) | (255 << 8) | 255;
            effect_settings.effect_main_color = (255 << 16) | (255 << 8) | 255;
        }
        else
        {
            effect_settings.effect_id = 1;
            effect_settings.effect_start_pixel = 0;
            effect_settings.effect_end_pixel = NUM_LEDS;
            effect_settings.effect_accent_pixel = 0;

            // TODO: add conversion from HUE to RGB
            // latest_config_.led_hue;

            effect_settings.effect_accent_color = (128 << 16) | (0 << 8) | 128;
            effect_settings.effect_main_color = (128 << 16) | (0 << 8) | 128;
        }
        led_ring_task_->setEffect(effect_settings);

        // latest_config_.led_hue
        // led_ring_task_->setEffect(0, 0, 0, NUM_LEDS, 0, (blue << 16) | (green << 8) | red, (blue << 16) | (green << 8) | red);
    }

    // #if SK_LEDS
    //     for (uint8_t i = 0; i < NUM_LEDS; i++)
    //     {
    //         leds[i].setHSV(latest_config_.led_hue, 255 - 180 * CLAMP(press_value_unit, (float)0, (float)1) - 75 * pressed, brightness >> 8);

    //         // Gamma adjustment
    //         leds[i].r = dim8_video(leds[i].r);
    //         leds[i].g = dim8_video(leds[i].g);
    //         leds[i].b = dim8_video(leds[i].b);
    //     }
    //     FastLED.show();
    // #endif
}

void AppTask::setConfiguration(Configuration *configuration)
{
    SemaphoreGuard lock(mutex_);
    configuration_ = configuration;
}

QueueHandle_t AppTask::getConnectivityStateQueue()
{
    return connectivity_status_queue_;
}

QueueHandle_t AppTask::getSensorsStateQueue()
{
    return sensors_status_queue_;
}

QueueHandle_t AppTask::getAppSyncQueue()
{
    return app_sync_queue_;
}

void AppTask::addListener(QueueHandle_t queue)
{
    listeners_.push_back(queue);
}

void AppTask::publish(const AppState &state)
{
    for (auto listener : listeners_)
    {
        xQueueOverwrite(listener, &state);
    }
}

void AppTask::publishState()
{
    // Apply local state before publishing to serial
    latest_state_.press_nonce = press_count_;
    current_protocol_->handleState(latest_state_);
}

void AppTask::applyConfig(PB_SmartKnobConfig config, bool from_remote)
{
    remote_controlled_ = from_remote;
    latest_config_ = config;
    motor_task_.setConfig(config);
}
