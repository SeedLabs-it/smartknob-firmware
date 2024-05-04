#include "root_task.h"
#include "semaphore_guard.h"
#include "util.h"

QueueHandle_t trigger_motor_calibration_;
uint8_t trigger_motor_calibration_event_;

// this is global function because we don't have better design yet
void delete_me_TriggerMotorCalibration()
{
    uint8_t trigger = 1;
    xQueueSendToBack(trigger_motor_calibration_, &trigger, 0);
}

RootTask::RootTask(
    const uint8_t task_core,
    Configuration *configuration,
    MotorTask &motor_task,
    DisplayTask *display_task,
    WifiTask *wifi_task,
    MqttTask *mqtt_task,
    LedRingTask *led_ring_task,
    SensorsTask *sensors_task,
    ResetTask *reset_task) : Task("RootTask", 1024 * 16, ESP_TASK_MAIN_PRIO, task_core),
                             stream_(),
                             configuration_(configuration),
                             motor_task_(motor_task),
                             display_task_(display_task),
                             wifi_task_(wifi_task),
                             mqtt_task_(mqtt_task),
                             led_ring_task_(led_ring_task),
                             sensors_task_(sensors_task),
                             reset_task_(reset_task),
                             plaintext_protocol_(
                                 stream_, [this]()
                                 { motor_task_.runCalibration(); },
                                 [this]()
                                 { sensors_task_->factoryStrainCalibrationCallback(); },
                                 [this]()
                                 {
                                     sensors_task_->weightMeasurementCallback();
                                 }),
                             proto_protocol_(
                                 stream_,
                                 configuration,
                                 [this](PB_SmartKnobConfig &config)
                                 { applyConfig(config, true); },
                                 [this]()
                                 { motor_task_.runCalibration(); },
                                 [this]()
                                 { sensors_task_->factoryStrainCalibrationCallback(); })

{
#if SK_DISPLAY
    assert(display_task != nullptr);
#endif

    trigger_motor_calibration_ = xQueueCreate(1, sizeof(uint8_t *));
    assert(trigger_motor_calibration_ != NULL);

    app_sync_queue_ = xQueueCreate(2, sizeof(cJSON *));
    assert(app_sync_queue_ != NULL);

    // log_queue_ = xQueueCreate(10, sizeof(std::string *));
    // assert(log_queue_ != NULL);

    knob_state_queue_ = xQueueCreate(1, sizeof(PB_SmartKnobState));
    assert(knob_state_queue_ != NULL);

    connectivity_status_queue_ = xQueueCreate(1, sizeof(ConnectivityState));
    assert(connectivity_status_queue_ != NULL);

    sensors_status_queue_ = xQueueCreate(100, sizeof(SensorsState));
    assert(sensors_status_queue_ != NULL);

    mutex_ = xSemaphoreCreateMutex();
    assert(mutex_ != NULL);
}

RootTask::~RootTask()
{
    vSemaphoreDelete(mutex_);
}

void RootTask::setHassApps(HassApps *apps)
{
    this->hass_apps = apps;
}

void RootTask::run()
{
    uint8_t task_started_at = millis();
    stream_.begin();

    motor_task_.addListener(knob_state_queue_);

    plaintext_protocol_.init([this]()
                             {
                                 //  CHANGE MOTOR CONFIG????
                             },
                             [this]()
                             {
                                 OSConfiguration *os_config = this->configuration_->getOSConfiguration();

                                 switch (os_config->mode)
                                 {
                                 case Onboarding:
                                     os_config->mode = Demo;
                                     display_task_->enableDemo();
                                     //  CHANGE MOTOR CONFIG
                                     break;
                                 case Demo:
                                     os_config->mode = Hass;
                                     display_task_->enableHass();
                                     //  CHANGE MOTOR CONFIG

                                     break;
                                 case Hass:
                                     os_config->mode = Onboarding;
                                     display_task_->enableOnboarding();
                                     break;
                                 default:
                                     os_config->mode = Hass;
                                     display_task_->enableHass();
                                     //  CHANGE MOTOR CONFIG

                                     break;
                                 }

                                 this->configuration_->saveOSConfigurationInMemory(*os_config);
                             });

    // Start in legacy protocol mode
    current_protocol_ = &plaintext_protocol_;
    Logging::getInstance().setLogger(&plaintext_protocol_);

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
            LOGE("Unknown protocol requested: %d", protocol);
            break;
        }
        Logging::getInstance().setLogger(current_protocol_);
    };

    plaintext_protocol_.setProtocolChangeCallback(protocol_change_callback);
    proto_protocol_.setProtocolChangeCallback(protocol_change_callback);

    MotorNotifier motor_notifier = MotorNotifier([this](PB_SmartKnobConfig config)
                                                 { applyConfig(config, false); });

    os_config_notifier_.setCallback([this](OSMode os_mode)
                                    {
                                        OSConfiguration *os_config = this->configuration_->getOSConfiguration();
                                        os_config->mode = os_mode;
                                        this->configuration_->saveOSConfiguration(*os_config);

                                        switch (os_config->mode)
                                        {
                                        case Onboarding:
                                            display_task_->enableOnboarding();
                                            break;
                                        case Demo:
                                            display_task_->enableDemo();
                                            break;
                                        case Hass:
                                            display_task_->enableHass();
                                            break;
                                        default:
                                            break;
                                        } });

    // waiting for config to be loaded
    bool is_configuration_loaded = false;
    while (!is_configuration_loaded)
    {
        LOGI("Waiting for configuration");
        xSemaphoreTake(mutex_, portMAX_DELAY);
        is_configuration_loaded = configuration_ != nullptr;
        xSemaphoreGive(mutex_);
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    configuration_->setSharedEventsQueue(wifi_task_->getWiFiEventsQueue());

    sensors_task_->setSharedEventsQueue(wifi_task_->getWiFiEventsQueue());

    reset_task_->setSharedEventsQueue(wifi_task_->getWiFiEventsQueue());

    display_task_->getOnboardingFlow()->setMotorUpdater(&motor_notifier);
    display_task_->getOnboardingFlow()->setOSConfigNotifier(&os_config_notifier_);
#if SK_WIFI
    wifi_task_->setConfig(configuration_->getWiFiConfiguration());
    display_task_->getOnboardingFlow()->setWiFiNotifier(wifi_task_->getNotifier());

    display_task_->getErrorHandlingFlow()->setSharedEventsQueue(wifi_task_->getWiFiEventsQueue());
#if SK_MQTT
    mqtt_task_->setConfig(configuration_->getMQTTConfiguration());
    mqtt_task_->setSharedEventsQueue(wifi_task_->getWiFiEventsQueue());
#endif
#endif

    display_task_->getErrorHandlingFlow()->setMotorNotifier(&motor_notifier);
    display_task_->getDemoApps()->setMotorNotifier(&motor_notifier);
    display_task_->getDemoApps()->setOSConfigNotifier(&os_config_notifier_);
    display_task_->getHassApps()->setMotorNotifier(&motor_notifier);

    // TODO: move playhaptic to notifier? or other interface to just pass "possible" motor commands not entire object/class.
    reset_task_->setMotorTask(&motor_task_);

    switch (configuration_->getOSConfiguration()->mode)
    {
    case Onboarding:
        display_task_->enableOnboarding();
        break;
    case Demo:
        os_config_notifier_.setOSMode(Onboarding);
        // display_task_->enableOnboarding();
        break;
    case Hass:
        display_task_->enableHass();
        break;

    default:
        break;
    }

    motor_notifier.loopTick();

    EntityStateUpdate entity_state_update_to_send;

    // Value between [0, 65536] for brightness when not engaging with knob
    bool isCurrentSubPositionSet = false;
    float currentSubPosition;
    WiFiEvent wifi_event;

    AppState app_state = {};
    while (1)
    {

        if (xQueueReceive(trigger_motor_calibration_, &trigger_motor_calibration_event_, 0) == pdTRUE)
        {
            motor_task_.runCalibration();
        }
#if SK_WIFI
        if (xQueueReceive(wifi_task_->getWiFiEventsQueue(), &wifi_event, 0) == pdTRUE)
        {
            switch (configuration_->getOSConfiguration()->mode)
            {
            case Onboarding:
                display_task_->getOnboardingFlow()->handleEvent(wifi_event);
                break;
            case Demo:
                display_task_->getDemoApps()->handleEvent(wifi_event);
                break;
            case Hass:
                display_task_->getHassApps()->handleEvent(wifi_event);
                break;
            default:
                break;
            }

            switch (wifi_event.type)
            {
            case SK_WIFI_STA_CONNECTED_NEW_CREDENTIALS:
                WiFiConfiguration wifi_config;
                strcpy(wifi_config.ssid, wifi_event.body.wifi_sta_connected.ssid);
                strcpy(wifi_config.passphrase, wifi_event.body.wifi_sta_connected.passphrase);
                configuration_->saveWiFiConfiguration(wifi_config);
                break;
#if SK_MQTT
            case SK_RESET_ERROR:
                switch (configuration_->getOSConfiguration()->mode)
                {
                case Onboarding:
                    display_task_->enableOnboarding();
                    break;
                case Hass:
                    display_task_->enableHass();
                    break;
                default:
                    break;
                }
                wifi_task_->resetRetryCount();
                mqtt_task_->handleEvent(wifi_event);
                display_task_->getErrorHandlingFlow()->handleEvent(wifi_event); // if reset error or dismiss error is triggered elsewhere.
                break;
            case SK_WIFI_STA_CONNECTED:
                if (configuration_->getOSConfiguration()->mode == Hass)
                {
                    MQTTConfiguration mqtt_config = configuration_->getMQTTConfiguration();
                    mqtt_task_->getNotifier()->requestConnect(mqtt_config);
                }
                break;
            case SK_MQTT_STATE_UPDATE:
                display_task_->getHassApps()->handleEvent(wifi_event);
                break;
            case SK_DISMISS_ERROR:
                display_task_->getErrorHandlingFlow()->handleEvent(wifi_event);
                switch (configuration_->getOSConfiguration()->mode)
                {
                case Onboarding:
                    display_task_->enableOnboarding();
                    break;
                case Demo:
                    display_task_->enableDemo();
                    break;
                case Hass:
                    display_task_->enableHass();
                    break;
                default:
                    break;
                }
                break;
            case SK_RESET_BUTTON_PRESSED:
                app_state.screen_state.awake_until = millis() + 15000;
                app_state.screen_state.has_been_engaged = true;
            case SK_RESET_BUTTON_RELEASED:
                display_task_->getErrorHandlingFlow()
                    ->handleEvent(wifi_event);
                break;
            case SK_MQTT_CONNECTION_FAILED:
            case SK_MQTT_RETRY_LIMIT_REACHED:
            case SK_WIFI_STA_CONNECTION_FAILED:
            case SK_WIFI_STA_RETRY_LIMIT_REACHED:
                if (wifi_event.sent_at > task_started_at + 3000) // give stuff 3000ms to connect at start before displaying errors.
                {
                    display_task_->getErrorHandlingFlow()->handleEvent(wifi_event);
                }
                break;
            case SK_MQTT_NEW_CREDENTIALS_RECIEVED:
                mqtt_task_->getNotifier()->requestSetupAndConnect(wifi_event.body.mqtt_connecting);
                break;
            case SK_MQTT_CONNECTED_NEW_CREDENTIALS:
                configuration_->saveMQTTConfiguration(wifi_event.body.mqtt_connecting);
                wifi_task_->retryMqtt(true);     //! SUPER UGLY FIX/HACK, NEEDED TO REDIRECT USER IF MQTT CREDENTIALS FAILED
                wifi_task_->mqttConnected(true); //! SUPER UGLY FIX/HACK, NEEDED TO REDIRECT USER IF MQTT CREDENTIALS FAILED
                break;
            case SK_MQTT_TRY_NEW_CREDENTIALS_FAILED:
                wifi_task_->retryMqtt(true); //! SUPER UGLY FIX/HACK, NEEDED TO REDIRECT USER IF MQTT CREDENTIALS FAILED
                // wifi_task_->getNotifier()->requestRetryMQTT(); //! DOESNT WORK WITH NOTIFIER, NEEDS TO UPDATE BOOL, BUT WIFI_TASK IS IN LOOP WAITING FOR THIS BOOL TO CHANGE
                break;
            case SK_CONFIGURATION_SAVED:
                if (current_protocol_ == &proto_protocol_)
                {
                    proto_protocol_.sendInitialInfo();
                }
                break;
            case SK_STRAIN_CALIBRATION:
                if (current_protocol_ == &proto_protocol_)
                {
                    LOGD("Sending strain calib state.");
                    proto_protocol_.sendStrainCalibState(wifi_event.body.calibration_step);
                }
                break;
            default:
                mqtt_task_->handleEvent(wifi_event);
                break;

#endif
            }
        }
#endif
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
            if (app_state.screen_state.awake_until < millis())
            {
                app_state.screen_state.awake_until = millis() + 4000; // 1s
            }
        }
        // Check if the knob is awake, and if the time is expired
        // and set it to not engaged
        if (app_state.screen_state.has_been_engaged && millis() < app_state.screen_state.awake_until)
        {
            app_state.screen_state.has_been_engaged = false;
        }
#if SK_ALS
        // We are multiplying the current luminosity of the enviroment (0,1 range)
        // by the MIN LCD Brightness. This is for the case where we are not engaging with the knob.
        // If it's very dark around the knob we are dimming this to 0, otherwise we dim it in a range
        // [0, MIN_LCD_BRIGHTNESS]
        uint16_t targetLuminosity = static_cast<uint16_t>(round(latest_sensors_state_.illumination.lux_adj * app_state.screen_state.MIN_LCD_BRIGHTNESS));

        if (app_state.screen_state.has_been_engaged == false &&
            abs(app_state.screen_state.brightness - targetLuminosity) > 500 && // is the change substantial?
            millis() > app_state.screen_state.awake_until)
        {
            if ((app_state.screen_state.brightness < targetLuminosity))
            {
                app_state.screen_state.brightness = (targetLuminosity);
            }
            else
            {
                // TODO: I don't like this decay function. It's too slow for delta too small
                app_state.screen_state.brightness = app_state.screen_state.brightness - ((app_state.screen_state.brightness - targetLuminosity) / 8);
            }
        }
        else if (app_state.screen_state.has_been_engaged == false && (abs(app_state.screen_state.brightness - targetLuminosity) <= 500))
        {
            // in case we have very little variation of light, and the screen is not engaged, make sure we stay on a stable luminosity value
            app_state.screen_state.brightness = (targetLuminosity);
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
            LOGD("App sync requested!");
#if SK_MQTT // Should this be here??
            hass_apps->sync(mqtt_task_->getApps());

            LOGD("Giving 0.5s for Apps to initialize");
            delay(500);
            display_task_->getHassApps()->triggerMotorConfigUpdate();
            mqtt_task_->unlock();
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

            switch (configuration_->getOSConfiguration()->mode)
            {
            case Onboarding:
                entity_state_update_to_send = display_task_->getOnboardingFlow()->update(app_state);
                break;
            case Demo:
                entity_state_update_to_send = display_task_->getDemoApps()->update(app_state);
                break;
            case Hass:
                entity_state_update_to_send = hass_apps->update(app_state);
                break;
            default:
                break;
            }

#if SK_MQTT
            mqtt_task_->enqueueEntityStateToSend(entity_state_update_to_send);
#endif

            if (entity_state_update_to_send.play_haptic)
            {
                motor_task_.playHaptic(true, false);
            }

            publish(app_state);
            publishState();
        }

        current_protocol_->loop();

        // std::string *log_string;
        // while (xQueueReceive(log_queue_, &log_string, 0) == pdTRUE)
        // {
        //     // LOGI(log_string->c_str());
        //     delete log_string;
        // }

        motor_notifier.loopTick();
        os_config_notifier_.loopTick();

        updateHardware(app_state);

        delay(1);
    }
}

void RootTask::updateHardware(AppState app_state)
{
    static bool pressed;
#if SK_STRAIN

    if (configuration_loaded_)
    {
        switch (latest_sensors_state_.strain.virtual_button_code)
        {
        case VIRTUAL_BUTTON_SHORT_PRESSED:
            if (last_strain_pressed_played_ != VIRTUAL_BUTTON_SHORT_PRESSED)
            {
                LOGD("Handling short press");
                motor_task_.playHaptic(true, false);
                last_strain_pressed_played_ = VIRTUAL_BUTTON_SHORT_PRESSED;
            }
            /* code */
            break;
        case VIRTUAL_BUTTON_LONG_PRESSED:
            if (last_strain_pressed_played_ != VIRTUAL_BUTTON_LONG_PRESSED)
            {
                LOGD("Handling long press");

                motor_task_.playHaptic(true, true);
                last_strain_pressed_played_ = VIRTUAL_BUTTON_LONG_PRESSED;
                NavigationEvent event;
                event.press = NAVIGATION_EVENT_PRESS_LONG;

                //! GET ACTIVE FLOW? SO WE DONT HAVE DIFFERENT
                // display_task_->getActiveFlow()->handleNavigationEvent(event);
                switch (display_task_->getErrorHandlingFlow()->getErrorType())
                {
                case NO_ERROR:
                    switch (configuration_->getOSConfiguration()->mode)
                    {
                    case Onboarding:
                        display_task_->getOnboardingFlow()->handleNavigationEvent(event);
                        break;
                    case Demo:
                        display_task_->getDemoApps()->handleNavigationEvent(event);
                        break;
                    case Hass:
                        display_task_->getHassApps()->handleNavigationEvent(event);
                    default:
                        break;
                    }
                    break;
                case MQTT_ERROR:
                case WIFI_ERROR:
                    display_task_->getErrorHandlingFlow()->handleNavigationEvent(event);
                    break;
                default:
                    break;
                }
            }
            break;
        case VIRTUAL_BUTTON_SHORT_RELEASED:
            if (last_strain_pressed_played_ != VIRTUAL_BUTTON_SHORT_RELEASED)
            {
                LOGD("Handling short press released");

                motor_task_.playHaptic(false, false);
                last_strain_pressed_played_ = VIRTUAL_BUTTON_SHORT_RELEASED;
                NavigationEvent event;
                event.press = NAVIGATION_EVENT_PRESS_SHORT;
                switch (display_task_->getErrorHandlingFlow()->getErrorType())
                {
                case NO_ERROR:
                    switch (configuration_->getOSConfiguration()->mode)
                    {
                    case Onboarding:
                        display_task_->getOnboardingFlow()->handleNavigationEvent(event);
                        break;
                    case Demo:
                        display_task_->getDemoApps()->handleNavigationEvent(event);
                        break;
                    case Hass:
                        display_task_->getHassApps()->handleNavigationEvent(event);
                    default:
                        break;
                    }
                    break;
                case MQTT_ERROR:
                case WIFI_ERROR:
                    display_task_->getErrorHandlingFlow()->handleNavigationEvent(event);
                    break;
                default:
                    break;
                }
            }
            break;
        case VIRTUAL_BUTTON_LONG_RELEASED:

            if (last_strain_pressed_played_ != VIRTUAL_BUTTON_LONG_RELEASED)
            {
                LOGD("Handling long press released");

                motor_task_.playHaptic(false, false);
                last_strain_pressed_played_ = VIRTUAL_BUTTON_LONG_RELEASED;
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
        EffectSettings effect_settings;
        // THERE ARE 3 potential range of the display
        // 1- Engaged
        // 2- Not Engaged and enviroment brightness is high
        // 3- Not engaged and enviroment brightness is low
        // The following code is coreographing the different led states as :
        // if 1: led ring is fully on
        // if 2: led ring is fully off.
        // if 3: we have 1 led on as beacon (also refered as lighthouse in other part of the code).

        if (brightness > app_state.screen_state.MIN_LCD_BRIGHTNESS)
        {
            // case 1. FADE-IN led
            effect_settings.effect_id = 4; // FADE-IN
            effect_settings.effect_start_pixel = 0;
            effect_settings.effect_end_pixel = NUM_LEDS;
            effect_settings.effect_accent_pixel = 0;

            // TODO: add conversion from HUE to RGB
            // latest_config_.led_hue;
            effect_settings.effect_main_color = (0 << 16) | (128 << 8) | 128;
            led_ring_task_->setEffect(effect_settings);
        }
        else if (brightness == app_state.screen_state.MIN_LCD_BRIGHTNESS)
        {
            // case 2. FADE-OUT led
            effect_settings.effect_id = 5;
            effect_settings.effect_start_pixel = 0;
            effect_settings.effect_end_pixel = NUM_LEDS;
            effect_settings.effect_accent_pixel = 0;
            effect_settings.effect_main_color = (0 << 16) | (128 << 8) | 128;
            led_ring_task_->setEffect(effect_settings);
        }
        else
        {
            // case 3 - Beacon
            effect_settings.effect_id = 2;
            effect_settings.effect_start_pixel = 0;
            effect_settings.effect_end_pixel = NUM_LEDS;
            effect_settings.effect_accent_pixel = 0;
            effect_settings.effect_main_color = (0 << 16) | (128 << 8) | 128;
            led_ring_task_->setEffect(effect_settings);
        }
        led_ring_task_->setEffect(effect_settings);

        // latest_config_.led_hue
        // led_ring_task_->setEffect(0, 0, 0, NUM_LEDS, 0, (blue << 16) | (green << 8) | red, (blue << 16) | (green << 8) | red);
    }

    // How far button is pressed, in range [0, 1]
    // float press_value_unit = 0;
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

void RootTask::loadConfiguration()
{
    SemaphoreGuard lock(mutex_);
    if (!configuration_loaded_)
    {
        if (configuration_ != nullptr)
        {
            configuration_value_ = configuration_->get();
            configuration_loaded_ = true;

            configuration_->loadOSConfiguration();

#if SK_WIFI
            if (configuration_->getOSConfiguration()->mode == Hass && configuration_->loadWiFiConfiguration())
            {

                WiFiConfiguration wifi_config = configuration_->getWiFiConfiguration();
                // TODO: send event to wifi to start STA part with given credentials
                wifi_task_->getNotifier()->requestSTA(wifi_config);
            }
#endif
#if SK_MQTT
            if (configuration_->getOSConfiguration()->mode == Hass && configuration_->loadMQTTConfiguration())
            {
                MQTTConfiguration mqtt_config = configuration_->getMQTTConfiguration();
                LOGD("MQTT_CONFIG: %s", mqtt_config.host);

                // mqtt_task_->setupMQTT(mqtt_config);
                // DO STUFF WITH MQTT CONFIG!!!
                // mqtt_task_->getNotifier()->requestConnect(mqtt_config); // ! DONT CONNECT MQTT HERE WAIT FOR WIFI TO CONNECT
            }
#endif
        }
    }
}

QueueHandle_t RootTask::getConnectivityStateQueue()
{
    return connectivity_status_queue_;
}

QueueHandle_t RootTask::getSensorsStateQueue()
{
    return sensors_status_queue_;
}

QueueHandle_t RootTask::getAppSyncQueue()
{
    return app_sync_queue_;
}

void RootTask::addListener(QueueHandle_t queue)
{
    listeners_.push_back(queue);
}

void RootTask::publish(const AppState &state)
{
    for (auto listener : listeners_)
    {
        xQueueOverwrite(listener, &state);
    }
}

void RootTask::publishState()
{
    // Apply local state before publishing to serial
    latest_state_.press_nonce = press_count_;
    current_protocol_->handleState(latest_state_);
}

void RootTask::applyConfig(PB_SmartKnobConfig config, bool from_remote)
{
    remote_controlled_ = from_remote;
    latest_config_ = config;
    motor_task_.setConfig(config);
}