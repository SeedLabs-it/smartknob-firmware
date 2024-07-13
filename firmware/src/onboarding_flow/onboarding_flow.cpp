#include "onboarding_flow.h"
#include <semaphore_guard.h>

OnboardingFlow::OnboardingFlow(SemaphoreHandle_t mutex) : mutex_(mutex)
{
    root_level_motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        ONBOARDING_FLOW_PAGE_COUNT - 1,
        35 * PI / 180,
        2,
        1,
        0.55,
        "",
        0,
        {},
        0,
        20,
    };

    blocked_motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        0,
        55 * PI / 180,
        0.01,
        0.6,
        1.1,
        "",
        0,
        {},
        0,
        90,
    };

    page_mgr = new OnboardingPageManager(main_screen, mutex);

    hass_flow = new HassOnboardingFlow(mutex, [this]()
                                       { this->render(); });

    // hass_flow->setWiFiNotifier(wifi_notifier);

    // page_mgr->add(WELCOME, new WelcomePage(main_screen));
    // // page_mgr->add(HASS, new HassPage());
    // // // page_mgr.add_screen(WIFI, new WiFiPage());
    // // page_mgr->add(DEMO, new DemoPage());
    // // page_mgr->add(ABOUT, new AboutPage());

    // page_mgr->show(WELCOME);

#ifdef RELEASE_VERSION
    sprintf(firmware_version, "%s", RELEASE_VERSION);
#else
    sprintf(firmware_version, "%s", "DEV");
#endif
}

OnboardingFlowPages getPageEnum(uint8_t screen)
{
    if (screen >= 0 && screen <= ONBOARDING_FLOW_PAGE_COUNT - 1)
    {
        return static_cast<OnboardingFlowPages>(screen);
    }
}

void OnboardingFlow::render()
{
    active_sub_menu = NONE;
    page_mgr->show(getPageEnum(current_position));

    {
        SemaphoreGuard lock(mutex_);
        lv_scr_load(main_screen);
    }
}

void OnboardingFlow::handleNavigationEvent(NavigationEvent event)
{
    if (active_sub_menu == HASS_SUB_MENU)
    {
        hass_flow->handleNavigationEvent(event);
        return;
    }
    if (active_sub_menu == NONE)
    {

        if (event.press == NAVIGATION_EVENT_PRESS_SHORT)
        {
            switch (getPageEnum(current_position))
            {
            case WELCOME_PAGE: // No submenus available for welcome page nor about page.
            case ABOUT_PAGE:
                break;
            case HASS_PAGE:
                wifi_notifier->requestAP();
                active_sub_menu = HASS_SUB_MENU;
                hass_flow->render();
                break;
            case DEMO_PAGE:
                os_config_notifier->setOSMode(OSMode::DEMO);
                break;

                break;
            default:
                break;
            }
        }
        return;
    }

    LOGE("Unhandled navigation event");
}

EntityStateUpdate OnboardingFlow::update(AppState state)
{
    // updateStateFromSystem(state);
    return updateStateFromKnob(state.motor_state);
}

EntityStateUpdate OnboardingFlow::updateStateFromKnob(PB_SmartKnobState state)
{
    if (current_position != state.current_position)
    {
        current_position = state.current_position;
        page_mgr->show(getPageEnum(current_position));
        // indicatorDots();
        // lv_scr_load(screens[current_position]);
        // CREATE TASK TO PREVENT WATCHDOG FROM TRIGGERING
        // xTaskCreate(screen_load_task, "Screen load task", 1024 * 3, main_screen, tskIDLE_PRIORITY, NULL);
    }

    EntityStateUpdate new_state;
    return new_state;
}

void OnboardingFlow::indicatorDots()
{
    // overlay = lv_obj_create(screens[WELCOME]);
    // lv_obj_t *indicator_dots[ONBOARDING_FLOW_PAGE_COUNT];
    // // Create new circles
    // for (uint8_t i = 0; i < ONBOARDING_FLOW_PAGE_COUNT; i++)
    // {
    //     lv_obj_t *dot;
    //     dot = drawCircle(overlay, 30);
    //     lv_obj_align(dot, LV_ALIGN_CENTER, 0, 0);
    //     lv_obj_set_style_bg_color(dot, LV_COLOR_MAKE(0x80, 0xFF, 0x50), 0);
    //     // lv_obj_set_style_bg_color(dot, LV_COLOR_MAKE(0xD9, 0xD9, 0xD9), 0);
    //     indicator_dots[i] = dot;
    // }
}

void OnboardingFlow::setMotorNotifier(MotorNotifier *motor_notifier)
{
    this->motor_notifier = motor_notifier;
    hass_flow->setMotorNotifier(motor_notifier); // TODO: BAD WAY... FIX
}

void OnboardingFlow::triggerMotorConfigUpdate()
{
    if (this->motor_notifier != nullptr)
    {
        motor_notifier->requestUpdate(root_level_motor_config);
    }
    else
    {
        LOGW("Motor_notifier is not set");
    }
}

void OnboardingFlow::setWiFiNotifier(WiFiNotifier *wifi_notifier)
{
    this->wifi_notifier = wifi_notifier;
}

void OnboardingFlow::setOSConfigNotifier(OSConfigNotifier *os_config_notifier)
{
    this->os_config_notifier = os_config_notifier;
    hass_flow->setOSConfigNotifier(os_config_notifier);
}

// #include "onboarding_flow.h"
// #include "apps/icons.h"

// OnboardingFlow::OnboardingFlow(TFT_eSprite *spr_, TFT_eSprite qrcode_spr_) : spr_(spr_), qrcode_spr_(qrcode_spr_)
// {
//     root_level_motor_config = PB_SmartKnobConfig{
//         0,
//         0,
//         0,
//         0,
//         4,
//         35 * PI / 180,
//         2,
//         1,
//         0.55,
//         "",
//         0,
//         {},
//         0,
//         20,
//     };

//     blocked_motor_config = PB_SmartKnobConfig{
//         0,
//         0,
//         0,
//         0,
//         0,
//         55 * PI / 180,
//         0.01,
//         0.6,
//         1.1,
//         "",
//         0,
//         {},
//         0,
//         90,
//     };

// #ifdef RELEASE_VERSION
//     sprintf(firmware_version, "%s", RELEASE_VERSION);
// #else
//     sprintf(firmware_version, "%s", "DEV");
// #endif
// }

// void OnboardingFlow::triggerMotorConfigUpdate()
// {
//     if (this->motor_notifier != nullptr)
//     {
//         motor_notifier->requestUpdate(root_level_motor_config);
//     }
//     else
//     {
//         LOGW("Motor_notifier is not set");
//     }
// }

// void OnboardingFlow::setMotorUpdater(MotorNotifier *motor_notifier)
// {
//     this->motor_notifier = motor_notifier;
// }

// void OnboardingFlow::setWiFiNotifier(WiFiNotifier *wifi_notifier)
// {
//     this->wifi_notifier = wifi_notifier;
// }

// void OnboardingFlow::setOSConfigNotifier(OSConfigNotifier *os_config_notifier)
// {
//     this->os_config_notifier = os_config_notifier;
// }

// EntityStateUpdate OnboardingFlow::update(AppState state)
// {
//     updateStateFromSystem(state);
//     return updateStateFromKnob(state.motor_state);
// }

// void OnboardingFlow::setQRCode(char *qr_data)
// {
//     QRCode qrcode;
//     uint8_t qrcodeVersion = 6;
//     int moduleSize = 2;

//     uint8_t qrcodeData[qrcode_getBufferSize(qrcodeVersion)];
//     qrcode_initText(&qrcode, qrcodeData, qrcodeVersion, 0, qr_data);

//     int qrCodeWidthHeight = qrcode.size * moduleSize;
//     qrcode_spr_.createSprite(qrCodeWidthHeight, qrCodeWidthHeight);
//     qrcode_spr_.fillSprite(TFT_BLACK);

//     for (uint8_t y = 0; y < qrcode.size; y++)
//     {
//         for (uint8_t x = 0; x < qrcode.size; x++)
//         {
//             if (qrcode_getModule(&qrcode, x, y))
//             {
//                 qrcode_spr_.fillRect(x * moduleSize, y * moduleSize, moduleSize, moduleSize, TFT_WHITE);
//             }
//         }
//     }
// }

// // TODO: rename to generic event
void OnboardingFlow::handleEvent(WiFiEvent event)
{

    // latest_event = event;
    switch (event.type)
    {
    case SK_WIFI_AP_STARTED:
    case SK_AP_CLIENT:
    case SK_WEB_CLIENT:
    case SK_WIFI_STA_TRY_NEW_CREDENTIALS:
    case SK_WIFI_STA_TRY_NEW_CREDENTIALS_FAILED:
    case SK_WEB_CLIENT_MQTT:
    case SK_MQTT_TRY_NEW_CREDENTIALS:
    case SK_MQTT_TRY_NEW_CREDENTIALS_FAILED:
    case SK_MQTT_CONNECTED_NEW_CREDENTIALS:
    case SK_MQTT_STATE_UPDATE:
        hass_flow->handleEvent(event);
        break;
    default:
        break;
    }
}

// void OnboardingFlow::handleNavigationEvent(NavigationEvent event)
// {

//     if (event.press == NAVIGATION_EVENT_PRESS_SHORT)
//     {
//         switch (current_page)
//         {
//         case ONBOARDING_FLOW_PAGE_STEP_HASS_1:
//             current_page = ONBOARDING_FLOW_PAGE_STEP_HASS_2;
//             if (!is_wifi_ap_started) // TODO: Make it actually use ap status, isnt updated if ap turns off. Or handle in wifi_task.
//             {
//                 wifi_notifier->requestAP();
//             }

//             motor_notifier->requestUpdate(blocked_motor_config);
//             break;
//         case ONBOARDING_FLOW_PAGE_STEP_DEMO_1:
//             os_config_notifier->setOSMode(Demo);
//             break;

//         default:
//             break;
//         }
//     }

//     if (event.press == NAVIGATION_EVENT_PRESS_LONG)
//     {
//         switch (current_page)
//         {
//         case ONBOARDING_FLOW_PAGE_STEP_HASS_2:
//             current_page = ONBOARDING_FLOW_PAGE_STEP_HASS_1;

//             motor_notifier->requestUpdate(root_level_motor_config);

//             break;

//         default:
//             break;
//         }
//     }
// }

// EntityStateUpdate OnboardingFlow::updateStateFromKnob(PB_SmartKnobState state)
// {
//     // TODO adapt for subviews if needed
//     current_position = state.current_position;

//     if (current_position >= 5)
//     {
//         current_position = current_page;
//     }

//     // this works only at the top menu
//     if (current_page < 5)
//     {
//         current_page = current_position;

//         // needed to next reload of App
//         root_level_motor_config.position_nonce = current_position;
//         root_level_motor_config.position = current_position;
//     }

//     root_level_motor_config.position_nonce = current_position;
//     root_level_motor_config.position = current_position;

//     EntityStateUpdate new_state;

//     return new_state;
// }

// void OnboardingFlow::updateStateFromSystem(AppState state) {}

// TFT_eSprite *OnboardingFlow::renderWelcomePage()
// {
//     uint16_t center_width = TFT_WIDTH / 2;
//     uint16_t center_height = TFT_HEIGHT / 2;
//     uint16_t icon_size = 96;

//     spr_->setTextDatum(CC_DATUM);

//     sprintf(buf_, "SMART KNOB");
//     spr_->setFreeFont(&NDS1210pt7b);
//     spr_->setTextColor(default_text_color);
//     spr_->drawString(buf_, center_width, center_height - 92, 1);

//     // TODO move this to a constant of software/hardware
//     sprintf(buf_, "DEV KIT V%d.%d", 0, 1);
//     spr_->setFreeFont(&NDS1210pt7b);
//     spr_->setTextColor(default_text_color);
//     spr_->drawString(buf_, center_width, center_height - 68, 1);

//     spr_->drawBitmap(center_width - icon_size / 2, (center_height - icon_size / 2) - 2, seedlabs_logo, icon_size, icon_size, TFT_WHITE, TFT_BLACK);

//     sprintf(buf_, "ROTATE TO START");
//     spr_->setFreeFont(&NDS1210pt7b);
//     spr_->setTextColor(accent_text_color);
//     spr_->drawString(buf_, center_width, center_height + 66, 1);

//     return this->spr_;
// }
// TFT_eSprite *OnboardingFlow::renderHass1StepPage()
// {
//     uint16_t center_width = TFT_WIDTH / 2;
//     uint16_t center_height = TFT_HEIGHT / 2;
//     uint16_t icon_size = 80;

//     spr_->setTextDatum(CC_DATUM);

//     sprintf(buf_, "HOME ASSISTANT");
//     spr_->setFreeFont(&NDS1210pt7b);
//     spr_->setTextColor(default_text_color);
//     spr_->drawString(buf_, center_width, center_height - 85, 1);

//     sprintf(buf_, "INTEGRATION");
//     spr_->setFreeFont(&NDS1210pt7b);
//     spr_->setTextColor(default_text_color);
//     spr_->drawString(buf_, center_width, center_height - 63, 1);

//     spr_->drawBitmap(center_width - icon_size / 2, center_height - icon_size / 2, home_assistant_80, icon_size, icon_size, TFT_WHITE, TFT_BLACK);

//     sprintf(buf_, "PRESS TO CONFIGURE");
//     spr_->setFreeFont(&NDS1210pt7b);
//     spr_->setTextColor(accent_text_color);
//     spr_->drawString(buf_, center_width, center_height + 66, 1);

//     return this->spr_;
// }
// TFT_eSprite *OnboardingFlow::renderHass2StepPage()
// {
//     uint8_t center = TFT_WIDTH / 2;

//     spr_->setTextDatum(CC_DATUM);

//     if (!is_wifi_ap_started)
//     {
//         sprintf(buf_, "WiFi starting...");
//         spr_->setFreeFont(&NDS1210pt7b);
//         spr_->setTextColor(accent_text_color);
//         spr_->drawString(buf_, center, center, 1);

//         sprintf(buf_, "wait a momoment please");
//         spr_->setFreeFont(&NDS125_small);
//         spr_->setTextColor(default_text_color);
//         spr_->drawString(buf_, center, center + 50, 1);

//         return this->spr_;
//     }

//     int8_t screen_name_label_h = spr_->fontHeight(1);

//     spr_->setTextDatum(CC_DATUM);
//     spr_->setTextSize(1);
//     spr_->setFreeFont(&NDS125_small);
//     spr_->setTextColor(accent_text_color);

//     spr_->drawString("SCAN TO CONNECT", center, screen_name_label_h * 3, 1);
//     spr_->drawString("TO THE SMART KNOB", center, screen_name_label_h * 4, 1);

//     uint8_t qrsize = qrcode_spr_.width();
//     qrcode_spr_.pushToSprite(spr_, center - qrsize / 2, center - qrsize / 2, TFT_BLACK);

//     spr_->drawString("OR CONNECT TO", center, TFT_HEIGHT - screen_name_label_h * 4, 1);
//     spr_->drawString(wifi_ap_ssid, center, TFT_HEIGHT - screen_name_label_h * 3, 1);
//     spr_->drawString(wifi_ap_passphrase, center, TFT_HEIGHT - screen_name_label_h * 2, 1);

//     return this->spr_;
// }

// TFT_eSprite *OnboardingFlow::renderHass3StepPage()
// {
//     uint16_t center_vertical = TFT_WIDTH / 2;
//     uint16_t center_horizontal = TFT_WIDTH / 2;

//     int8_t screen_name_label_h = spr_->fontHeight(1);

//     spr_->setTextDatum(CC_DATUM);
//     spr_->setTextSize(1);
//     spr_->setFreeFont(&NDS125_small);
//     spr_->setTextColor(accent_text_color);

//     spr_->drawString("SCAN TO START", center_horizontal, screen_name_label_h * 3, 1);
//     spr_->drawString("SETUP", center_horizontal, screen_name_label_h * 4, 1);

//     uint8_t qrsize = qrcode_spr_.width();
//     qrcode_spr_.pushToSprite(spr_, center_horizontal - qrsize / 2, center_vertical - qrsize / 2, TFT_BLACK);

//     char or_open[40];
//     sprintf(or_open, "OR OPEN: %s", ip_data);
//     spr_->drawString(or_open, center_horizontal, TFT_WIDTH - screen_name_label_h * 4, 1);
//     spr_->drawString("IN YOUR BROWSER", center_horizontal, TFT_WIDTH - screen_name_label_h * 3, 1);

//     return this->spr_;
// }
// TFT_eSprite *OnboardingFlow::renderHass4StepPage()
// {
//     uint16_t center_vertical = TFT_HEIGHT / 2;
//     uint16_t center_horizontal = TFT_WIDTH / 2;
//     int8_t screen_name_label_h = spr_->fontHeight(1);

//     spr_->setTextDatum(CC_DATUM);
//     spr_->setTextSize(1);

//     if (new_wifi_credentials_failed)
//     {
//         spr_->setFreeFont(&NDS125_small);
//         spr_->setTextColor(default_text_color);

//         spr_->drawString("Connection failed with", center_horizontal, center_vertical - screen_name_label_h * 3.7, 1);
//         spr_->drawString("provided credentials.", center_horizontal, center_vertical - screen_name_label_h * 3.2, 1);
//         spr_->drawString("Please try again.", center_horizontal, center_vertical - screen_name_label_h * 2.5, 1);
//     }

//     spr_->setFreeFont(&NDS1210pt7b);
//     spr_->setTextColor(accent_text_color);

//     spr_->drawString("CONTINUE IN", center_horizontal, center_vertical - screen_name_label_h, 1);
//     spr_->drawString("BROWSER", center_horizontal, center_vertical + screen_name_label_h, 1);

//     spr_->setTextColor(default_text_color);
//     spr_->drawString("WIFI", center_horizontal, TFT_HEIGHT - screen_name_label_h, 1);

//     return this->spr_;
// }
// TFT_eSprite *OnboardingFlow::renderHass5StepPage()
// {
//     uint16_t center_vertical = TFT_HEIGHT / 2;
//     uint16_t center_horizontal = TFT_WIDTH / 2;
//     int8_t screen_name_label_h = spr_->fontHeight(1);

//     spr_->setTextDatum(CC_DATUM);
//     spr_->setTextSize(1);
//     spr_->setFreeFont(&NDS1210pt7b);
//     spr_->setTextColor(accent_text_color);

//     spr_->drawString("CONNECTING TO", center_horizontal, center_vertical - screen_name_label_h, 1);
//     spr_->drawString(wifi_sta_ssid, center_horizontal, center_vertical + screen_name_label_h, 1);

//     sprintf(buf_, "%ds", max(0, 30 - (int)((millis() - latest_event.sent_at) / 1000))); // 30 = 10*3 should be same as wifi_client timeout in mqtt_task.cpp

//     spr_->setTextColor(default_text_color);

//     spr_->drawString(buf_, center_horizontal, screen_name_label_h, 1);

//     spr_->drawString("WIFI", center_horizontal, TFT_HEIGHT - screen_name_label_h, 1);

//     return this->spr_;
// }
// TFT_eSprite *OnboardingFlow::renderHass6StepPage()
// {
//     uint16_t center_vertical = TFT_HEIGHT / 2;
//     uint16_t center_horizontal = TFT_WIDTH / 2;
//     int8_t screen_name_label_h = spr_->fontHeight(1);

//     spr_->setTextDatum(CC_DATUM);
//     spr_->setTextSize(1);

//     if (new_mqtt_credentials_failed)
//     {
//         spr_->setFreeFont(&NDS125_small);
//         spr_->setTextColor(default_text_color);

//         spr_->drawString("Connection failed with", center_horizontal, center_vertical - screen_name_label_h * 3.7, 1);
//         spr_->drawString("provided credentials.", center_horizontal, center_vertical - screen_name_label_h * 3.2, 1);
//         spr_->drawString("Please try again.", center_horizontal, center_vertical - screen_name_label_h * 2.5, 1);
//     }

//     spr_->setFreeFont(&NDS1210pt7b);
//     spr_->setTextColor(accent_text_color);

//     spr_->drawString("CONTINUE IN", center_horizontal, center_vertical - screen_name_label_h, 1);
//     spr_->drawString("BROWSER", center_horizontal, center_vertical + screen_name_label_h, 1);

//     spr_->setTextColor(default_text_color);
//     spr_->drawString("MQTT", center_horizontal, TFT_HEIGHT - screen_name_label_h * 2, 1);

//     return this->spr_;
// }
// TFT_eSprite *OnboardingFlow::renderHass7StepPage()
// {
//     uint16_t center_vertical = TFT_HEIGHT / 2;
//     uint16_t center_horizontal = TFT_WIDTH / 2;
//     int8_t screen_name_label_h = spr_->fontHeight(1);

//     spr_->setTextDatum(CC_DATUM);
//     spr_->setTextSize(1);
//     spr_->setFreeFont(&NDS1210pt7b);
//     spr_->setTextColor(accent_text_color);

//     spr_->drawString("CONNECTING TO", center_horizontal, center_vertical - screen_name_label_h, 1);
//     spr_->drawString(mqtt_server, center_horizontal, center_vertical + screen_name_label_h, 1);

//     sprintf(buf_, "%ds", max(0, 30 - (int)((millis() - latest_event.sent_at) / 1000))); // 30=10*3 should be same as wifi_client timeout in mqtt_task.cpp

//     spr_->setTextColor(default_text_color);

//     spr_->drawString(buf_, center_horizontal, screen_name_label_h, 1);

//     spr_->drawString("MQTT", center_horizontal, TFT_HEIGHT - screen_name_label_h, 1);

//     return this->spr_;
// }

// TFT_eSprite *OnboardingFlow::renderHass8StepPage()
// {
//     uint16_t center_vertical = TFT_HEIGHT / 2;
//     uint16_t center_horizontal = TFT_WIDTH / 2;

//     int8_t screen_name_label_h = spr_->fontHeight(1);

//     spr_->setTextDatum(CC_DATUM);
//     spr_->setTextSize(2);
//     spr_->setFreeFont(&NDS125_small);
//     spr_->setTextColor(TFT_SKYBLUE);

//     spr_->drawString("CONTINUE THE SETUP IN", center_horizontal, center_vertical - screen_name_label_h / 1.8, 1);
//     spr_->drawString("HOME ASSISTANT", center_horizontal, center_vertical + screen_name_label_h / 1.8, 1);

//     return this->spr_;
// }

// TFT_eSprite *OnboardingFlow::renderWiFi1StepPage()
// {
//     uint16_t center_h = TFT_HEIGHT / 2;
//     uint16_t center_v = TFT_WIDTH / 2;
//     uint16_t icon_size = 80;

//     spr_->setTextDatum(CC_DATUM);

//     sprintf(buf_, "WIFI");
//     spr_->setFreeFont(&NDS1210pt7b);
//     spr_->setTextColor(default_text_color);
//     spr_->drawString(buf_, center_h, 50, 1);

//     spr_->drawBitmap(center_v - icon_size / 2, (center_h - icon_size / 2) - 2, wifi_conn_80, icon_size, icon_size, TFT_WHITE, TFT_BLACK);

//     sprintf(buf_, "PRESS TO CONFIGURE");
//     spr_->setFreeFont(&NDS1210pt7b);
//     spr_->setTextColor(accent_text_color);
//     spr_->drawString(buf_, center_h, 185, 1);

//     return this->spr_;
// }
// TFT_eSprite *OnboardingFlow::renderDemo1StepPage()
// {
//     uint16_t center_h = TFT_HEIGHT / 2;
//     uint16_t center_v = TFT_WIDTH / 2;
//     uint16_t icon_size = 80;
//     spr_->setTextDatum(CC_DATUM);

//     sprintf(buf_, "DEMO MODE");
//     spr_->setFreeFont(&NDS1210pt7b);
//     spr_->setTextColor(default_text_color);
//     spr_->drawString(buf_, center_h, 50, 1);

//     uint32_t colors[6] = {TFT_BLUE, TFT_PURPLE, TFT_RED, TFT_YELLOW, TFT_GREENYELLOW, TFT_GREEN};
//     float angle_step = 2 * PI / 6;
//     float circle_radius = 24; // Adjust as needed
//     uint8_t small_circle_radius = 8;

//     for (uint16_t i = 0; i < 6; i++)
//     {
//         int16_t x;
//         int16_t y;
//         x = center_h + circle_radius * sinf((i + 3) * angle_step);
//         y = center_h + circle_radius * cosf((i + 3) * angle_step);

//         spr_->fillCircle(x, y, small_circle_radius, colors[i]);
//     }

//     sprintf(buf_, "PRESS TO START");
//     spr_->setFreeFont(&NDS1210pt7b);
//     spr_->setTextColor(accent_text_color);
//     spr_->drawString(buf_, center_h, 185, 1);

//     return this->spr_;
// }
// TFT_eSprite *OnboardingFlow::renderAboutPage()
// {
//     uint16_t center_h = TFT_WIDTH / 2;
//     uint16_t center_v = TFT_HEIGHT / 2;

//     spr_->setTextDatum(CC_DATUM);
//     uint8_t left_padding = 30;

//     sprintf(buf_, "FIRMWARE: %s", firmware_version);
//     spr_->setFreeFont(&NDS1210pt7b);
//     spr_->setTextColor(default_text_color);
//     spr_->drawString(buf_, center_h, 85, 1);

//     sprintf(buf_, "HARDWARE: %s", "DEVKIT V0.1");
//     spr_->setFreeFont(&NDS1210pt7b);
//     spr_->setTextColor(default_text_color);
//     spr_->drawString(buf_, center_h, 115, 1);

//     sprintf(buf_, "WIFI: %s", "NOT CONNECTED");
//     spr_->setFreeFont(&NDS1210pt7b);
//     spr_->setTextColor(default_text_color);
//     spr_->drawString(buf_, center_h, 145, 1);

//     sprintf(buf_, "SEEDLABS.IT®");
//     spr_->setFreeFont(&NDS1210pt7b);
//     spr_->setTextColor(accent_text_color);
//     spr_->drawString(buf_, center_h, 175, 1);

//     return this->spr_;
// }

// TFT_eSprite *OnboardingFlow::render()
// {

//     if (is_onboarding_finished)
//     {
//         return renderHass8StepPage();
//     }

//     switch (current_page)
//     {
//     case ONBOARDING_FLOW_PAGE_STEP_WELCOME:
//         return renderWelcomePage();
//     case ONBOARDING_FLOW_PAGE_STEP_HASS_1:
//         return renderHass1StepPage();
//     case ONBOARDING_FLOW_PAGE_STEP_HASS_2:
//         return renderHass2StepPage();
//     case ONBOARDING_FLOW_PAGE_STEP_HASS_3:
//         return renderHass3StepPage();
//     case ONBOARDING_FLOW_PAGE_STEP_HASS_4:
//         return renderHass4StepPage();
//     case ONBOARDING_FLOW_PAGE_STEP_HASS_5:
//         return renderHass5StepPage();
//     case ONBOARDING_FLOW_PAGE_STEP_HASS_6:
//         return renderHass6StepPage();
//     case ONBOARDING_FLOW_PAGE_STEP_HASS_7:
//         return renderHass7StepPage();
//     case ONBOARDING_FLOW_PAGE_STEP_HASS_8:
//         return renderHass8StepPage();
//     case ONBOARDING_FLOW_PAGE_STEP_WIFI_1:
//         return renderWiFi1StepPage();
//     case ONBOARDING_FLOW_PAGE_STEP_DEMO_1:
//         return renderDemo1StepPage();
//     case ONBOARDING_FLOW_PAGE_STEP_ABOUT:
//         return renderAboutPage();

//     default:
//         current_page = ONBOARDING_FLOW_PAGE_STEP_WELCOME;
//         return renderWelcomePage();
//         break;
//     }

//     return this->spr_;
// };