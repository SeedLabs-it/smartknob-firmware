#pragma once
#include <WiFi.h>
#include <TFT_eSPI.h>

// #include "apps/app.h"

#include "util.h"
#include "display/page_manager.h"
#include "navigation/navigation.h"
#include "notify/motor_notifier/motor_notifier.h"
#include "notify/wifi_notifier/wifi_notifier.h"

#include "./pages/error.h"
#include "./pages/reset.h"

#include "font/NDS1210pt7b.h"
#include "font/NDS125_small.h"

// struct CurrentErrorState
// {
//     unsigned long start_ms;

//     lv_obj_t *error_label;
//     lv_obj_t *error_event_label;
//     lv_obj_t *error_type_label;

//     lv_obj_t *parent;
//     lv_obj_t *page;
// };

struct ErrorState
{
    ErrorType latest_error_type;
    WiFiEvent latest_event;

    unsigned long event_at;
};

enum ErrorPages
{
    ERROR_PAGE,
    RESET_PAGE,
    COUNT
};

class ErrorHandlingPageManager : public PageManager<ErrorPages>
{
public:
    ErrorHandlingPageManager(lv_obj_t *parent, SemaphoreHandle_t mutex) : PageManager<ErrorPages>(parent, mutex)
    {
        add(ErrorPages::ERROR_PAGE, new ErrorPage(parent));
        add(ErrorPages::RESET_PAGE, new ResetPage(parent));
    }

    void render(ErrorPages page_enum)
    {
        PageManager::show(page_enum);
        lv_scr_load(parent_);
    }
};

class ErrorHandlingFlow
{
public:
    ErrorHandlingFlow(SemaphoreHandle_t mutex);

    // void setQRCode(char *qr_data);
    void handleNavigationEvent(NavigationEvent event);
    void handleEvent(WiFiEvent event);
    void setMotorNotifier(MotorNotifier *motor_notifier);
    void setWiFiNotifier(WiFiNotifier *wifi_notifier);

    void setSharedEventsQueue(QueueHandle_t shared_event_queue);
    void publishEvent(WiFiEvent event);

    ErrorType getErrorType();

private:
    SemaphoreHandle_t mutex_;

    ErrorHandlingPageManager *page_manager = nullptr;

    lv_timer_t *timer;

    char buf_[64];

    int32_t current_position = 0;
    ErrorType error_type = NO_ERROR;

    ErrorState error_state = {
        NO_ERROR,
        SK_NO_EVENT};

    uint16_t default_text_color = rgbToUint32(150, 150, 150);
    uint16_t accent_text_color = rgbToUint32(128, 255, 80);

    PB_SmartKnobConfig root_level_motor_config;
    PB_SmartKnobConfig blocked_motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        0,
        PI,
        0,
        1,
        0.5,
        "",
        0,
        {},
        0,
        0,
    };

    MotorNotifier *motor_notifier;
    WiFiNotifier *wifi_notifier;

    QueueHandle_t shared_events_queue;

    char ap_data[64];
    char ip_data[64];
};