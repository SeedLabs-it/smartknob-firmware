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

#include "apps/onboarding_flow/onboarding_flow.h"

const uint8_t BOOT_MODE_NOT_SET = 0;
const uint8_t BOOT_MODE_ONBOARDING = 1;
const uint8_t BOOT_MODE_HASS = 2;

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
    void enableHass();

    OnboardingFlow *getOnboardingFlow();

protected:
    void run();

private:
    TFT_eSPI tft_ = TFT_eSPI();

    /** Full-size sprite used as a framebuffer */
    TFT_eSprite spr_ = TFT_eSprite(&tft_);
    Onboarding onboarding;
    HassApps hass_apps;

    // TODO: put under private
    OnboardingFlow onboarding_flow;

    QueueHandle_t app_state_queue_;

    AppState app_state_;
    SemaphoreHandle_t mutex_;
    uint16_t brightness_;
    Logger *logger_;
    void log(const char *msg);
    char buf_[128];

    uint8_t boot_mode = BOOT_MODE_NOT_SET;
};

#else

class DisplayTask
{
};

#endif
