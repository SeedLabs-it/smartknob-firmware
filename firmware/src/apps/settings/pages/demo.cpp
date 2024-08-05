#include "demo.h"

DemoSettingsPage::DemoSettingsPage(lv_obj_t *parent) : BasePage(parent)
{
    uint8_t dot_dia = 24;
    uint8_t distance_between = dot_dia / 2 + 4;
    uint8_t y_offset = 34;

    lv_obj_t *blue_colored_dot = lvDrawCircle(dot_dia, page);
    lv_obj_align(blue_colored_dot, LV_ALIGN_CENTER, 0, -(distance_between * 2) - y_offset);
    lv_obj_set_style_bg_color(blue_colored_dot, LV_COLOR_MAKE(0x28, 0xAD, 0xF7), LV_PART_MAIN);

    lv_obj_t *turquoise_colored_dot = lvDrawCircle(dot_dia, page);
    lv_obj_align(turquoise_colored_dot, LV_ALIGN_CENTER, distance_between * 2, -distance_between - y_offset);
    lv_obj_set_style_bg_color(turquoise_colored_dot, LV_COLOR_MAKE(0x53, 0xC8, 0xBA), LV_PART_MAIN);

    lv_obj_t *green_colored_dot = lvDrawCircle(dot_dia, page);
    lv_obj_align(green_colored_dot, LV_ALIGN_CENTER, distance_between * 2, distance_between - y_offset);
    lv_obj_set_style_bg_color(green_colored_dot, LV_COLOR_MAKE(0x8F, 0xEC, 0x67), LV_PART_MAIN);

    lv_obj_t *yellow_colored_dot = lvDrawCircle(dot_dia, page);
    lv_obj_align(yellow_colored_dot, LV_ALIGN_CENTER, 0, distance_between * 2 - y_offset);
    lv_obj_set_style_bg_color(yellow_colored_dot, LV_COLOR_MAKE(0xEF, 0xEE, 0x00), LV_PART_MAIN);

    lv_obj_t *red_colored_dot = lvDrawCircle(dot_dia, page);
    lv_obj_align(red_colored_dot, LV_ALIGN_CENTER, -(distance_between * 2), distance_between - y_offset);
    lv_obj_set_style_bg_color(red_colored_dot, LV_COLOR_MAKE(0xFE, 0x43, 0x44), LV_PART_MAIN);

    lv_obj_t *purple_colored_dot = lvDrawCircle(dot_dia, page);
    lv_obj_align(purple_colored_dot, LV_ALIGN_CENTER, -(distance_between * 2), -distance_between - y_offset);
    lv_obj_set_style_bg_color(purple_colored_dot, LV_COLOR_MAKE(0x9D, 0x58, 0xF5), LV_PART_MAIN);

    prompt_label = lv_label_create(page);
    lv_label_set_text(prompt_label, "PRESS TO START");
    lv_obj_align(prompt_label, LV_ALIGN_CENTER, 0, 52);
    lv_obj_set_style_text_font(prompt_label, &nds12_14px, 0);
    lv_obj_set_style_text_color(prompt_label, LV_COLOR_MAKE(0x80, 0xFF, 0x50), LV_STATE_DEFAULT);
}

void DemoSettingsPage::updateFromSystem(AppState state)
{
    state_ = state;
    if (state.os_mode_state == DEMO)
    {
        lv_label_set_text(prompt_label, "DISABLED");
    }
    else
    {
        lv_label_set_text(prompt_label, "PRESS TO START");
    }
}

void DemoSettingsPage::handleNavigation(NavigationEvent event)
{
    LOGE("DemoSettingsPage::handleNavigation");
    switch (event)
    {
    case NavigationEvent::SHORT:
        if (os_config_notifier_ != nullptr)
        {
            if (state_.os_mode_state != DEMO)
            {
                os_config_notifier_->setOSMode(OSMode::DEMO);
            }
            else
            {
                LOGV(PB_LogLevel_INFO, "Entering Demo Mode from Demo Mode is disabled.");
            }
        }
        else
        {
            LOGE("os_config_notifier_ isnt set.");
        }
        break;
    default:
        break;
    }
}

void DemoSettingsPage::setOSConfigNotifier(OSConfigNotifier *os_config_notifier)
{
    os_config_notifier_ = os_config_notifier;
}