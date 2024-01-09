#include "stopwatch.h"

StopwatchApp::StopwatchApp(TFT_eSprite *spr_, std::string entity_name) : App(spr_)
{

    this->entity_name = entity_name;

    motor_config = PB_SmartKnobConfig{
        0,
        0,
        4,
        0,
        0,
        60 * PI / 180,
        0.01,
        0.6,
        1.1,
        "Return-to-center",
        0,
        {},
        0,
        45,
    };

    big_icon = stopwatch_80;
    small_icon = stopwatch_40;
    friendly_name = "Stopwatch";
}

uint8_t StopwatchApp::navigationNext()
{
    // back to menu
    return 0;
}

EntityStateUpdate StopwatchApp::updateStateFromKnob(PB_SmartKnobState state)
{
    current_position = state.current_position;
    sub_position_unit = state.sub_position_unit;
    // current_volume = current_volume_position * 5;

    // needed to next reload of App
    motor_config.position_nonce = state.current_position;
    motor_config.position = state.current_position;

    EntityStateUpdate new_state;

    if (started && sub_position_unit < -3)
    {
        new_state.play_haptic = true;
        started = false;
    }

    if (!started && sub_position_unit > 3)
    {
        started = true;
        start_ms = millis();

        new_state.play_haptic = true;
    }

    // new_state.entity_name = entity_name;
    // new_state.new_value = current_volume * 1.0;

    // if (last_volume != current_volume)
    // {
    //     last_volume = current_volume;
    //     new_state.changed = true;
    //     sprintf(new_state.app_slug, "%s", APP_SLUG_MUSIC);
    // }

    return new_state;
}

void StopwatchApp::updateStateFromSystem(AppState state) {}

TFT_eSprite *StopwatchApp::render()
{

    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;
    uint16_t screen_radius = TFT_WIDTH / 2;

    unsigned long now = millis();

    uint16_t DISABLED_COLOR = spr_->color565(71, 71, 71);

    uint32_t background = spr_->color565(0, 0, 0);
    spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, background);

    // ESP_LOGD("stopwatch", "%d", percent);

    if (started)
    {
        uint16_t percent = uint16_t((-sub_position_unit) / 3.0 * 50);

        uint32_t notify_background = spr_->color565(255, 0, 0);

        if (sub_position_unit < -0.2 && sub_position_unit > -3)
        {
            spr_->fillRect(0, center_v + 70 + (50 - percent), TFT_WIDTH, TFT_HEIGHT, notify_background);
        }
        else if (sub_position_unit < -3)
        {
            spr_->fillRect(0, center_v + 70, TFT_WIDTH, TFT_HEIGHT, notify_background);
        }
    }
    else
    {
        uint16_t percent = uint16_t((sub_position_unit) / 3.0 * 50);

        uint32_t notify_background = spr_->color565(0, 255, 0);
        if (sub_position_unit > 0.2 && sub_position_unit < 3)
        {
            spr_->fillRect(0, center_v + 70 + (50 - percent), TFT_WIDTH, TFT_HEIGHT, notify_background);
        }
        else if (sub_position_unit > 3)
        {
            spr_->fillRect(0, center_v + 70, TFT_WIDTH, TFT_HEIGHT, notify_background);
        }
    }

    unsigned long diff_ms = now - start_ms;
    unsigned long stopwatch_ms = 0;
    unsigned long stopwatch_sec = 0;
    unsigned long stopwatch_hour = 0;

    if (started)
    {
        stopwatch_ms = diff_ms % 100;
        stopwatch_sec = floor((diff_ms / 1000) % 60);
        stopwatch_hour = floor((diff_ms / (1000 * 60)) % 60);
    }

    spr_->setTextColor(TFT_WHITE);
    spr_->setFreeFont(&Roboto_Light_60);

    char buf_[16];
    spr_->setTextDatum(TR_DATUM);
    sprintf(buf_, "%02d:%02d.", stopwatch_hour, stopwatch_sec);
    spr_->drawString(buf_, center_h + 80, center_v - 30, 1);

    spr_->setFreeFont(&Roboto_Thin_24);

    spr_->setTextDatum(TL_DATUM);
    sprintf(buf_, "%02d", stopwatch_ms);
    spr_->drawString(buf_, center_h + 80 + 5, center_v, 1);

    if (started && sub_position_unit < -2)
    {
        sprintf(buf_, "reset?");

        spr_->setTextDatum(CC_DATUM);
        spr_->setFreeFont(&Roboto_Thin_24);
        spr_->drawString(buf_, center_h, center_v + 90, 1);
    }

    if (sub_position_unit < -3)
    {
        sprintf(buf_, "reseted");

        spr_->setTextDatum(CC_DATUM);
        spr_->setFreeFont(&Roboto_Thin_24);
        spr_->drawString(buf_, center_h, center_v + 90, 1);
    }

    if (sub_position_unit > 3)
    {
        sprintf(buf_, "started");

        spr_->setTextDatum(CC_DATUM);
        spr_->setFreeFont(&Roboto_Thin_24);
        spr_->drawString(buf_, center_h, center_v + 90, 1);
    }
    else if (!started && sub_position_unit > 2)
    {
        sprintf(buf_, "start?");

        spr_->setTextDatum(CC_DATUM);
        spr_->setFreeFont(&Roboto_Thin_24);
        spr_->drawString(buf_, center_h, center_v + 90, 1);
    }

    return this->spr_;
};