#pragma once

#if SK_DISPLAY

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <HTTPClient.h>

#include "proto_gen/smartknob.pb.h"
#include "task.h"
#include "app_config.h"

#include "apps/apps.h"
#include "apps/hass/hass_apps.h"
#include "apps/demo/demo_apps.h"

#include "onboarding_flow/onboarding_flow.h"
#include "error_handling_flow/error_handling_flow.h"

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
    void setApps(Apps apps);
    OnboardingFlow *getOnboardingFlow();
    DemoApps *getDemoApps();
    HassApps *getHassApps();

    void enableOnboarding();
    void enableHass();
    void enableDemo();

    ErrorHandlingFlow *getErrorHandlingFlow();

    void enableErrorHandlingFlow();

protected:
    void run();

private:
    TFT_eSPI tft_ = TFT_eSPI();

    /** Full-size sprite used as a framebuffer */
    TFT_eSprite spr_ = TFT_eSprite(&tft_);

    OnboardingFlow onboarding_flow = OnboardingFlow(&spr_, TFT_eSprite(&tft_));
    DemoApps demo_apps;
    HassApps hass_apps;
    ErrorHandlingFlow error_handling_flow = ErrorHandlingFlow(&spr_, TFT_eSprite(&tft_));

    QueueHandle_t app_state_queue_;

    AppState app_state_;
    SemaphoreHandle_t mutex_;
    uint16_t brightness_;
    char buf_[128];

    OSMode os_mode;
    ErrorType error_type;
};

#else

class DisplayTask
{
};

#endif
