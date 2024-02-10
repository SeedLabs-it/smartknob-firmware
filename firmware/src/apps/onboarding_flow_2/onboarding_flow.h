#pragma once

// #include <Arduino.h>
// #include <TFT_eSPI.h>

// #include <Arduino.h>
// #include <TFT_eSPI.h>

// #include "../font/NDS1210pt7b.h"
// #include "../font/roboto_thin_20.h"
// #include "../font/roboto_thin_20.h"

// #include "../proto_gen/smartknob.pb.h"
// #include "../app_config.h"

// #include "../app.h"

// class OnboardingFlow
// {
// public:
//     OnboardingFlow(TFT_eSprite *spr_);
//     TFT_eSprite *render();
//     EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
//     void updateStateFromSystem(AppState state);

//     // TODO: add event
//     void handleNavigationEvent();

// private:
//     TFT_eSprite *spr_;
//     PB_SmartKnobConfig motor_config;

//     int32_t current_position = 0;
//     uint8_t num_positions;
//     char buf_[24];
//     ConnectivityState connectivity_state;
//     ProximityState proximity_state;
//     // char room[24];
//     // char fw_version[24];
//     // // needed for UI
//     // float adjusted_sub_position = 0;

//     // // screen tearing test
//     // unsigned long startup_ms;
//     // unsigned long startup_diff_ms;
//     // uint16_t screen_fps;

//     // // motor calibration
//     // bool motor_calibration_started = false;
//     // bool motor_calibration_event_sent = false;
//     // bool motor_calibration_finished = false;
//     // unsigned long motor_calibration_requested_ms;           // for count down
//     // unsigned long motor_calibration_expected_event_send_ms; // for count down
//     // unsigned long motor_calibration_expected_finish_ms;     // for count down
//     // uint8_t calibration_time_s = 40;
//     // uint8_t calibration_safety_time_s = 5;

//     // void renderViewMotorCalibration();
// };