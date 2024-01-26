#pragma once

#if SK_DISPLAY

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <HTTPClient.h>

#include "logger.h"
#include "proto_gen/smartknob.pb.h"
#include "task.h"
#include "app_config.h"

#include "apps/apps.h"
#include "apps/onboarding/onboarding.h"
#include "apps/hass/hass_apps.h"

class DisplayTask : public Task<DisplayTask>
{
    friend class Task<DisplayTask>; // Allow base Task to invoke protected run()

public:
    DisplayTask(const uint8_t task_core);
    ~DisplayTask();

    QueueHandle_t getKnobStateQueue();

    void setBrightness(uint16_t brightness);
    void setLogger(Logger *logger);
    void setApps(Apps apps);
    HassApps *getHassApps();
    Onboarding *getOnboarding();
    void enableOnboarding();
    void disableOnboarding();

protected:
    void run();

private:
    TFT_eSPI tft_ = TFT_eSPI();

    /** Full-size sprite used as a framebuffer */
    TFT_eSprite spr_ = TFT_eSprite(&tft_);
    Onboarding onboarding;
    HassApps hass_apps;

    QueueHandle_t app_state_queue_;

    AppState app_state_;
    SemaphoreHandle_t mutex_;
    uint16_t brightness_;
    Logger *logger_;
    void log(const char *msg);
    char buf_[128];
    bool is_onboarding;
};

#else

class DisplayTask
{
};

#endif
