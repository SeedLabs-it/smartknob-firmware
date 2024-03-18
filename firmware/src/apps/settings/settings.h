#pragma once
#include "../app.h"

#include "../../font/NDS1210pt7b.h"
#include "../../font/roboto_thin_20.h"
#include "../../font/roboto_thin_20.h"

class SettingsApp : public App
{
public:
    SettingsApp(TFT_eSprite *spr_);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);

protected:
    int8_t navigationNext();
    // int8_t navigationBack();

private:
    // uint8_t current_volume = 0;
    // uint8_t current_volume_position = 0;

    int32_t current_position = 0;
    uint8_t num_positions;
    ConnectivityState connectivity_state;
    char ip_address[20];
    char ssid[128];

    ProximityState proximity_state;
    char room[24];
    char fw_version[24];
    // needed for UI
    float adjusted_sub_position = 0;
    char buf_[48];

    // screen tearing test
    unsigned long startup_ms;
    unsigned long startup_diff_ms;
    uint16_t screen_fps;

    // motor calibration
    bool motor_calibration_started = false;
    bool motor_calibration_event_sent = false;
    bool motor_calibration_finished = false;
    unsigned long motor_calibration_requested_ms;           // for count down
    unsigned long motor_calibration_expected_event_send_ms; // for count down
    unsigned long motor_calibration_expected_finish_ms;     // for count down
    uint8_t calibration_time_s = 40;
    uint8_t calibration_safety_time_s = 5;

    void renderViewMotorCalibration();
};