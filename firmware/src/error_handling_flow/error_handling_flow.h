#pragma once
#include <WiFi.h>

#include "util.h"
#include "display/page_manager.h"
#include "navigation/navigation.h"
#include "notify/motor_notifier/motor_notifier.h"
#include "notify/wifi_notifier/wifi_notifier.h"

#include "./pages/error.h"
#include "./pages/reset.h"

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

    void handleNavigationEvent(NavigationEvent event);
    void handleEvent(WiFiEvent event);
    void setMotorNotifier(MotorNotifier *motor_notifier);
    void setWiFiNotifier(WiFiNotifier *wifi_notifier);

    void setEnabled(bool enabled);

    void setSharedEventsQueue(QueueHandle_t shared_event_queue);
    void publishEvent(WiFiEvent event);

    ErrorType getErrorType();

private:
    SemaphoreHandle_t mutex_;

    bool enabled = true;

    ErrorHandlingPageManager *page_manager = nullptr;

    lv_timer_t *timer;

    char buf_[64];

    int32_t current_position = 0;
    ErrorType error_type = NO_ERROR;

    ErrorState error_state = {
        NO_ERROR,
        SK_NO_EVENT,
        1,
    };

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