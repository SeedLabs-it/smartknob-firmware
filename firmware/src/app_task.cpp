#include "app_task.h"
#include "semaphore_guard.h"
#include "util.h"

AppTask::AppTask(
    const uint8_t task_core,
    MotorTask &motor_task,
    DisplayTask *display_task,
    NetworkingTask *networking_task,
    LedRingTask *led_ring_task,
    SensorsTask *sensors_task) : Task("App", 4800, 1, task_core),
                                 stream_(),
                                 motor_task_(motor_task),
                                 display_task_(display_task),
                                 networking_task_(networking_task),
                                 led_ring_task_(led_ring_task),
                                 sensors_task_(sensors_task),
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

void AppTask::setHassApps(HassApps *apps)
{
    this->hass_apps = apps;
}

void AppTask::setOnboardingApps(Onboarding *apps)
{
    this->onboarding_apps = apps;
}

void AppTask::strainCalibrationCallback()
{
    if (!configuration_loaded_)
    {
        log("Strain calibration step 0: configuration not loaded, exiting");

        return;
    }
    if (strain_calibration_step_ == 0)
    {
        log("Strain calibration step 1: Don't touch the knob, then press 'S' again");
        strain_calibration_step_ = 1;
    }
    else if (strain_calibration_step_ == 1)
    {
        configuration_value_.strain.idle_value = latest_sensors_state_.strain.raw_value;
        snprintf(buf_, sizeof(buf_), "  idle_value=%d", configuration_value_.strain.idle_value);
        log(buf_);
        log("Strain calibration step 2: Push and hold down the knob with medium pressure, and press 'S' again");
        strain_calibration_step_ = 2;
    }
    else if (strain_calibration_step_ == 2)
    {

        configuration_value_.strain.press_delta = latest_sensors_state_.strain.raw_value - configuration_value_.strain.idle_value;
        configuration_value_.has_strain = true;

        ESP_LOGD("1", "pre-save %d", configuration_value_.strain.idle_value);
        snprintf(buf_, sizeof(buf_), "  press_delta=%d", configuration_value_.strain.press_delta);
        log(buf_);
        log("Strain calibration complete! Saving...");

        ESP_LOGD("2", "pre-save %d", configuration_value_.strain.idle_value);
        strain_calibration_step_ = 0;

        sensors_task_->updateStrainCalibration(configuration_value_.strain.idle_value, configuration_value_.strain.press_delta);

        if (configuration_->setStrainCalibrationAndSave(configuration_value_.strain))
        {
            log("  Saved!");
        }
        else
        {
            log("  FAILED to save config!!!");
        }
    }
}

void AppTask::verboseToggleCallback()
{
    sensors_task_->toggleVerbose();
}

void AppTask::run()
{
    stream_.begin();

    log("Giving 0.5s for Apps to initialize");
    delay(500);

    // TODO: make this configurable based on config
    display_task_->enableOnboarding();

    changeConfig(MENU);
    motor_task_.addListener(knob_state_queue_);

    // TODO: remove me
    std::string apps_config = "[{\"app_slug\":\"stopwatch\",\"app_id\":\"stopwatch.office\",\"friendly_name\":\"Stopwatch\",\"area\":\"office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"light_switch\",\"app_id\":\"light.ceiling\",\"friendly_name\":\"Ceiling\",\"area\":\"Kitchen\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"light_dimmer\",\"app_id\":\"light.workbench\",\"friendly_name\":\"Workbench\",\"area\":\"Kitchen\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"thermostat\",\"app_id\":\"climate.office\",\"friendly_name\":\"Climate\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"3d_printer\",\"app_id\":\"3d_printer.office\",\"friendly_name\":\"3D Printer\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"blinds\",\"app_id\":\"blinds.office\",\"friendly_name\":\"Shades\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"},{\"app_slug\":\"music\",\"app_id\":\"music.office\",\"friendly_name\":\"Music\",\"area\":\"Office\",\"menu_color\":\"#ffffff\"}]";
    cJSON *json_root = cJSON_Parse(apps_config.c_str());
    hass_apps->sync(json_root);

    plaintext_protocol_.init([this]()
                             { changeConfig(MENU); },
                             [this]()
                             {
                                 this->strainCalibrationCallback();
                             },
                             [this]()
                             {
                                 this->verboseToggleCallback();
                             },
                             [this]()
                             {
                                 this->is_onboarding = !this->is_onboarding;
                                 this->is_onboarding ? display_task_->enableOnboarding() : display_task_->disableOnboarding();
                                 changeConfig(MENU);
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
            app_state.screen_state.awake_until = millis() + 4000; // 1s
        }
        // Check if the knob is awake, and if the time is expired
        // and set it to not engaged
        if (app_state.screen_state.has_been_engaged && millis() < app_state.screen_state.awake_until)
        {
            app_state.screen_state.has_been_engaged = false;
        }
#if SK_ALS
        float targetLuminosity = latest_sensors_state_.illumination.lux_adj * app_state.screen_state.MIN_LCD_BRIGHTNESS;
        if (app_state.screen_state.has_been_engaged == false &&
            abs(app_state.screen_state.brightness - targetLuminosity) > 500 && // is the change substantial?
            millis() > app_state.screen_state.awake_until)
        {
            if (app_state.screen_state.brightness < (targetLuminosity))
            {
                app_state.screen_state.brightness = (targetLuminosity);
            }
            else
            {
                // TODO: I don't like this decay function. It's too slow for delta too small
                app_state.screen_state.brightness = app_state.screen_state.brightness - ((app_state.screen_state.brightness - targetLuminosity) / 8);
            }
        }
#endif
#if !SK_ALS
        if (app_state.screen_state.has_been_engaged == false)
        {
            app_state.screen_state.brightness = app_state.screen_state.MAX_LCD_BRIGHTNESS;
        }
#endif

        if (xQueueReceive(connectivity_status_queue_, &latest_connectivity_state_, 0) == pdTRUE)
        {
            app_state.connectivity_state = latest_connectivity_state_;
        }

        if (xQueueReceive(app_sync_queue_, &apps_, 0) == pdTRUE)
        {
            ESP_LOGD("app_task", "App sync requested!");
#if SK_NETWORKING // Should this be here??
            hass_apps->sync(networking_task_->getApps());

            log("Giving 0.5s for Apps to initialize");
            delay(500);

            changeConfig(MENU);
            networking_task_->unlock();
#endif
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

            if (is_onboarding)
            {
                entity_state_update_to_send = onboarding_apps->update(app_state);
            }
            else
            {
                entity_state_update_to_send = hass_apps->update(app_state);
            }

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

void AppTask::changeConfig(int8_t id)
{
    if (id == DONT_NAVIGATE)
    {
        return;
    }

    if (is_onboarding)
    {
        onboarding_apps->setActive(id);
        applyConfig(onboarding_apps->getActiveMotorConfig(), false);
    }
    else
    {
        hass_apps->setActive(id);
        applyConfig(hass_apps->getActiveMotorConfig(), false);
    }
}

void AppTask::updateHardware(AppState app_state)
{
    // How far button is pressed, in range [0, 1]
    float press_value_unit = 0;

    static bool pressed;
#if SK_STRAIN

    if (configuration_loaded_ && configuration_value_.has_strain && strain_calibration_step_ == 0)
    {
        // Ignore readings that are way out of expected bounds

        switch (latest_sensors_state_.strain.virtual_button_code)
        {
        case VIRTUAL_BUTTON_SHORT_PRESSED:
            if (last_strain_pressed_played_ != VIRTUAL_BUTTON_SHORT_PRESSED)
            {
                motor_task_.playHaptic(true);
                last_strain_pressed_played_ = VIRTUAL_BUTTON_SHORT_PRESSED;
            }
            /* code */
            break;
        case VIRTUAL_BUTTON_LONG_PRESSED:
            if (last_strain_pressed_played_ != VIRTUAL_BUTTON_LONG_PRESSED)
            {
                motor_task_.playHaptic(true);
                motor_task_.playHaptic(true);
                last_strain_pressed_played_ = VIRTUAL_BUTTON_LONG_PRESSED;
            }
            /* code */
            break;
        case VIRTUAL_BUTTON_SHORT_RELEASED:
            if (last_strain_pressed_played_ != VIRTUAL_BUTTON_SHORT_RELEASED)
            {
                motor_task_.playHaptic(false);
                last_strain_pressed_played_ = VIRTUAL_BUTTON_SHORT_RELEASED;
                /* code */
                if (is_onboarding)
                {
                    changeConfig(onboarding_apps->navigationNext());
                }
                else
                {
                    changeConfig(hass_apps->navigationNext());
                }
            }
            break;
        case VIRTUAL_BUTTON_LONG_RELEASED:
            /* code */
            if (last_strain_pressed_played_ != VIRTUAL_BUTTON_LONG_RELEASED)
            {
                motor_task_.playHaptic(false);
                last_strain_pressed_played_ = VIRTUAL_BUTTON_LONG_RELEASED;

                if (is_onboarding)
                {
                    changeConfig(onboarding_apps->navigationBack());
                }
                else
                {
                    changeConfig(hass_apps->navigationBack());
                }
            }
            break;
        default:
            last_strain_pressed_played_ = VIRTUAL_BUTTON_IDLE;
            break;
        }
    }

#endif

    uint16_t brightness = UINT16_MAX;
// TODO: brightness scale factor should be configurable (depends on reflectivity of surface)
#if SK_ALS
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
            effect_settings.effect_main_color = (0 << 16) | (128 << 8) | 0;
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
    if (!configuration_loaded_)
    {
        if (configuration_ != nullptr)
        {
            configuration_value_ = configuration_->get();
            configuration_loaded_ = true;
            sensors_task_->updateStrainCalibration(configuration_value_.strain.idle_value, configuration_value_.strain.press_delta);
        }
    }
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
