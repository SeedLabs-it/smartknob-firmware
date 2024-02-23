#include "stopwatch.h"

#include <cstring>

#ifdef USE_DISPLAY_BUFFER
StopwatchApp::StopwatchApp(std::string entity_name) : App()
#else
StopwatchApp::StopwatchApp(TFT_eSprite *spr_, std::string entity_name) : App(spr_)
#endif
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

void StopwatchApp::clear()
{
    for (int i = 0; i < laps_max; i++)
    {
        laps[i] = LapTime{};
    }
    last_lap_added = 0;
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
        clear();
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

int8_t StopwatchApp::navigationNext()
{
    if (last_lap_added >= laps_max)
    {
        return DONT_NAVIGATE;
    }

    if (started)
    {
        unsigned long now = millis();
        uint32_t diff_ms = now - start_ms; // diff will not be that big ever

        uint32_t lap_ms = diff_ms;

        if (last_lap_added > 0)
        {
            lap_ms = diff_ms - laps[last_lap_added - 1].raw_ms;
        }

        uint32_t stopwatch_ms = 0;
        uint32_t stopwatch_sec = 0;
        uint32_t stopwatch_min = 0;

        stopwatch_ms = lap_ms % 100;
        stopwatch_sec = floor((lap_ms / 1000) % 60);
        stopwatch_min = floor((lap_ms / (1000 * 60)) % 60);

        // ESP_LOGD("stopwatch", "diff: %d", diff);
        laps[last_lap_added].m = stopwatch_min;
        laps[last_lap_added].s = stopwatch_sec;
        laps[last_lap_added].ms = stopwatch_ms;
        laps[last_lap_added].raw_ms = diff_ms;
        laps[last_lap_added].lap_ms = lap_ms;

        if (last_lap_added > 0)
        {
            laps[last_lap_added].improvement = int32_t(laps[last_lap_added].lap_ms) - int32_t(laps[last_lap_added - 1].lap_ms);
        }

        last_lap_added++;
    }

    return DONT_NAVIGATE;
}

#ifdef USE_DISPLAY_BUFFER
void StopwatchApp::render()
#else
TFT_eSprite *StopwatchApp::render()
#endif
{

    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;
    uint16_t screen_radius = TFT_WIDTH / 2;

    unsigned long now = millis();

    uint16_t DISABLED_COLOR = spr_->color565(71, 71, 71);

    uint32_t background = spr_->color565(0, 0, 0);
    spr_->fillRect(0, 0, TFT_WIDTH, TFT_HEIGHT, background);

    // ESP_LOGD("stopwatch", "%d", percent);

    uint8_t rendering_lap = 0;
    // render laps

    int32_t lap_impeovement = -INT32_MAX;

    for (int i = laps_max - 1; i >= 0; i--)
    {
        if (laps[i].m != 0 || laps[i].s != 0 || laps[i].ms != 0)
        {
            spr_->setTextColor(TFT_WHITE);
            sprintf(buf_, "lap %d - %02d:%02d.%02d", i + 1,
                    laps[i].m,
                    laps[i].s,
                    laps[i].ms);

            spr_->setTextDatum(CL_DATUM);
            spr_->setFreeFont(&Roboto_Thin_24);
            spr_->drawString(buf_, 42, center_v + 34 + rendering_lap * 26, 1);

            if (lap_impeovement == -INT32_MAX)
            {
                lap_impeovement = laps[i].improvement;

                int32_t lap_improvement_ms = abs(laps[i].improvement % 100);
                int32_t lap_improvementh_sec = abs(floor((laps[i].improvement / 1000) % 60));
                int32_t lap_improvement_min = abs(floor((laps[i].improvement / (1000 * 60)) % 60));

                char sign = '+';
                spr_->setTextColor(TFT_RED);
                if (laps[i].improvement < 0)
                {
                    spr_->setTextColor(TFT_GREEN);
                    sign = '-';
                }

                sprintf(buf_, "%c%02d:%02d.%02d", sign, lap_improvement_min, lap_improvementh_sec, lap_improvement_ms);

                spr_->setTextDatum(CC_DATUM);
                spr_->setFreeFont(&Roboto_Thin_24);
                spr_->drawString(buf_, center_h, center_v - 65, 1);
            }

            rendering_lap++;
        }
    }
    spr_->setTextColor(TFT_WHITE);

    if (sub_position_unit < 0)
    {
        float percent = -(sub_position_unit) / 3.0;

        uint8_t color_channel = percent > 1 ? 255 : percent * 255;

        uint32_t notify_background = spr_->color565(color_channel, 0, 0);

        if (sub_position_unit < -0.2 && sub_position_unit > -3)
        {
            spr_->fillRect(0, center_v + 70 + (50 - percent * 50), TFT_WIDTH, TFT_HEIGHT, notify_background);
        }
        else if (sub_position_unit < -3)
        {
            spr_->fillRect(0, center_v + 70 + (50 - 1 * 50), TFT_WIDTH, TFT_HEIGHT, notify_background);
        }
    }
    else if (sub_position_unit > 0)
    {
        float percent = (sub_position_unit) / 3.0;

        uint8_t color_channel = percent > 1 ? 255 : percent * 255;

        uint32_t notify_background = spr_->color565(0, color_channel, 0);
        if (sub_position_unit > 0.2 && sub_position_unit < 3)
        {
            spr_->fillRect(0, center_v + 70 + (50 - percent * 50), TFT_WIDTH, TFT_HEIGHT, notify_background);
        }
        else if (sub_position_unit > 3)
        {
            spr_->fillRect(0, center_v + 70 + (50 - 1 * 50), TFT_WIDTH, TFT_HEIGHT, notify_background);
        }
    }

    // if (started)
    // {
    //     uint16_t percent = uint16_t((-sub_position_unit) / 3.0 * 50);

    //     uint32_t notify_background = spr_->color565(100, 0, 0);

    //     if (sub_position_unit < -0.2 && sub_position_unit > -3)
    //     {
    //         spr_->fillRect(0, center_v + 70 + (50 - percent), TFT_WIDTH, TFT_HEIGHT, notify_background);
    //     }
    //     else if (sub_position_unit < -3)
    //     {
    //         notify_background = spr_->color565(255, 0, 0);
    //         spr_->fillRect(0, center_v + 70 + (50 - percent), TFT_WIDTH, TFT_HEIGHT, notify_background);
    //     }
    // }
    // else
    // {
    //     uint16_t percent = uint16_t((sub_position_unit) / 3.0 * 50);

    //     uint32_t notify_background = spr_->color565(0, 100, 0);
    //     if (sub_position_unit > 0.2 && sub_position_unit < 3)
    //     {
    //         spr_->fillRect(0, center_v + 70 + (50 - percent), TFT_WIDTH, TFT_HEIGHT, notify_background);
    //     }
    //     else if (sub_position_unit > 3)
    //     {
    //         notify_background = spr_->color565(0, 255, 0);
    //         spr_->fillRect(0, center_v + 70 + (50 - percent), TFT_WIDTH, TFT_HEIGHT, notify_background);
    //     }
    // }

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

    spr_->setTextDatum(TR_DATUM);
    sprintf(buf_, "%02d:%02d.", stopwatch_hour, stopwatch_sec);
    spr_->drawString(buf_, center_h + 80, center_v - 30, 1);

    spr_->setFreeFont(&Roboto_Thin_24);

    spr_->setTextDatum(TL_DATUM);
    sprintf(buf_, "%02d", stopwatch_ms);
    spr_->drawString(buf_, center_h + 80 + 5, center_v, 1);

    if (started && sub_position_unit < -2.2)
    {
        sprintf(buf_, "reset?");

        spr_->setTextDatum(CC_DATUM);
        spr_->setFreeFont(&Roboto_Thin_24);
        spr_->drawString(buf_, center_h, center_v + 90, 1);
    }

    if (sub_position_unit < -3 && !started)
    {
        sprintf(buf_, "reset");

        spr_->setTextDatum(CC_DATUM);
        spr_->setFreeFont(&Roboto_Thin_24);
        spr_->drawString(buf_, center_h, center_v + 90, 1);
    }

    if (sub_position_unit > 3 && started)
    {
        spr_->setTextColor(TFT_BLACK);

        sprintf(buf_, "started");

        spr_->setTextDatum(CC_DATUM);
        spr_->setFreeFont(&Roboto_Thin_24);
        spr_->drawString(buf_, center_h, center_v + 90, 1);
    }
    else if (!started && sub_position_unit > 2.2)
    {
        spr_->setTextColor(TFT_BLACK);

        sprintf(buf_, "start?");

        spr_->setTextDatum(CC_DATUM);
        spr_->setFreeFont(&Roboto_Thin_24);
        spr_->drawString(buf_, center_h, center_v + 90, 1);
    }

    #ifndef USE_DISPLAY_BUFFER
        return this->spr_;
    #endif
};