#pragma once
#include "../app.h"
#include "../util.h"
class LightSwitchApp : public App
{
public:
    LightSwitchApp(SemaphoreHandle_t mutex, char *app_id, char *friendly_name, char *entity_id);
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromHASS(MQTTStateUpdate mqtt_state_update);
    void updateStateFromSystem(AppState state);

protected:
    void initScreen()
    {
        SemaphoreGuard lock(mutex_);

        arc_ = lv_arc_create(screen);
        lv_obj_set_size(arc_, 220, 220);
        lv_arc_set_rotation(arc_, 225);
        lv_arc_set_bg_angles(arc_, 0, 90);
        lv_arc_set_knob_offset(arc_, 0);
        lv_arc_set_value(arc_, 0);
        lv_obj_center(arc_);

        lv_obj_set_style_arc_opa(arc_, LV_OPA_0, LV_PART_MAIN);
        lv_obj_set_style_arc_opa(arc_, LV_OPA_0, LV_PART_INDICATOR);
        lv_obj_set_style_bg_color(arc_, LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), LV_PART_KNOB);

        lv_obj_set_style_arc_width(arc_, 2, LV_PART_INDICATOR);

        light_bulb = lv_img_create(screen);
        lv_img_set_src(light_bulb, &big_icon);
        lv_obj_set_style_image_recolor_opa(light_bulb, LV_OPA_COVER, 0);
        lv_obj_set_style_image_recolor(light_bulb, LV_COLOR_MAKE(0xFF, 0xFF, 0xFF), 0);

        lv_obj_center(light_bulb);

        lv_obj_t *label = lv_label_create(screen);
        lv_label_set_text(label, friendly_name);
        lv_obj_align_to(label, light_bulb, LV_ALIGN_OUT_BOTTOM_MID, 0, 12);
    };

private:
    lv_image_dsc_t big_icon_active;

    lv_obj_t *arc_;
    lv_obj_t *light_bulb;

    uint8_t current_position = 0;
    uint8_t last_position = 0;
    uint8_t num_positions = 0;

    // needed for UI
    float sub_position_unit = 0;
    float adjusted_sub_position = 0;
    float old_adjusted_sub_position = 0;
    bool first_run = false;
};