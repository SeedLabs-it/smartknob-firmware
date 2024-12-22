#pragma once

#if SK_DISPLAY

#include "./display/lv_conf.h"
#include "./display/driver/lv_skdk.h"
#include "lvgl.h"

#include <Arduino.h>
#include <HTTPClient.h>

#include "proto/proto_gen/smartknob.pb.h"
#include "task.h"
#include "app_config.h"

#include "./apps/demo_apps.h"
#include "./apps/spotify_standalone.h"

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
    OnboardingFlow *getOnboardingFlow();
    DemoApps *getDemoApps();
    HassApps *getHassApps();
    SpotifyStandalone *getSpotifyStandalone();

    void enableOnboarding();
    void enableHass();
    void enableDemo();
    void enableSpotify();

    ErrorHandlingFlow *getErrorHandlingFlow();

    void enableErrorHandlingFlow();

protected:
    void run();

private:
    OnboardingFlow *onboarding_flow = nullptr;
    DemoApps *demo_apps = nullptr;
    HassApps *hass_apps = nullptr;
    SpotifyStandalone *spotify_standalone = nullptr;
    ErrorHandlingFlow *error_handling_flow = nullptr;

    QueueHandle_t app_state_queue_;

    AppState app_state_;
    SemaphoreHandle_t mutex_;
    uint16_t brightness_ = UINT16_MAX;
    char buf_[128];

    OSMode display_os_mode = UNSET;
    ErrorType error_type;
};

#else

class DisplayTask
{
};

#endif
