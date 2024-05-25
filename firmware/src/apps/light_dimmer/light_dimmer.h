#pragma once
#include "../app.h"
#include "../../util.h"

// #include "../../font/NDS1210pt7b.h"
// #include "../../font/Pixel62mr11pt7b.h"

const uint8_t LIGHT_DIMMER_APP_MODE_DIMMER = 0;
const uint8_t LIGHT_DIMMER_APP_MODE_HUE = 1;

class LightDimmerApp : public App
{
public:
    LightDimmerApp(SemaphoreHandle_t mutex, char *app_id, char *friendly_name, char *entity_id);

    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state) override;
    void updateStateFromHASS(MQTTStateUpdate mqtt_state_update) override;

protected:
    int8_t navigationNext();
    int8_t navigationBack();

private:
    void initScreen() override
    {
        SemaphoreGuard lock(mutex_);

        arc_ = lv_arc_create(screen);
        lv_obj_set_size(arc_, 236, 236);
        lv_arc_set_rotation(arc_, 150);
        lv_arc_set_bg_angles(arc_, 0, 240);
        lv_arc_set_knob_offset(arc_, 0);
        lv_arc_set_value(arc_, 0);
        lv_obj_center(arc_);

        lv_obj_set_style_bg_opa(arc_, LV_OPA_0, LV_PART_KNOB);

        lv_obj_set_style_arc_color(arc_, LV_COLOR_MAKE(0xF5, 0xA4, 0x42), LV_PART_INDICATOR);

        lv_obj_set_style_arc_width(arc_, 18, LV_PART_MAIN);
        lv_obj_set_style_arc_width(arc_, 18, LV_PART_INDICATOR);

        percentage_label_ = lv_label_create(screen);
        char buf_[16];
        sprintf(buf_, "%d%%", current_brightness);
        lv_label_set_text(percentage_label_, buf_);
        lv_obj_set_style_text_font(percentage_label_, &EIGHTTWOXC_48px, 0);
        lv_obj_align(percentage_label_, LV_ALIGN_CENTER, 0, -12);

        lv_obj_t *friendly_name_label = lv_label_create(screen);
        lv_label_set_text(friendly_name_label, friendly_name);
        lv_obj_align_to(friendly_name_label, percentage_label_, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);
    }

    void initHueScreen()
    {
        SemaphoreGuard lock(mutex_);

        lv_obj_set_style_bg_color(screen, LV_COLOR_MAKE(0xFF, 0x00, 0xFF), 0);

        lv_obj_t *img = lv_img_create(screen);
        LV_IMAGE_DECLARE(a4_transp_mask);
        lv_img_set_src(img, &a4_transp_mask);
        lv_obj_set_width(img, a4_transp_mask.header.w);
        lv_obj_set_height(img, a4_transp_mask.header.h);
        lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    }

    lv_obj_t *arc_;
    lv_obj_t *percentage_label_;

    int16_t current_position = 0;
    int16_t last_position = 0;
    uint8_t num_positions = 0;

    // needed for UI
    float sub_position_unit = 0;
    float adjusted_sub_position = 0;

    cJSON *json;

    // app state
    uint8_t app_state_mode = LIGHT_DIMMER_APP_MODE_DIMMER;

    bool color_set = true;

    uint16_t app_hue_position = 0;
    uint8_t current_brightness = 0;
    bool is_on = false;

    bool first_run = false;

    uint16_t calculateAppHuePosition(uint16_t position);

    lv_obj_t *hue_screen;
};