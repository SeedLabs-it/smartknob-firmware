#include "onboarding.h"

OnboardingApp::OnboardingApp(TFT_eSprite *spr_) : App(spr_)
{
    // sprintf(room, "%s", "");

    motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        0, // max position < min position indicates no bounds
        60 * PI / 180,
        1,
        1,
        0.55,
        "SKDEMO_Onboarding", // TODO: clean this
        0,
        {},
        0,
        20,
    };
}

EntityStateUpdate OnboardingApp::updateStateFromKnob(PB_SmartKnobState state)
{
    // TODO: cache menu size

    int32_t position_for_onboarding_calc = state.current_position;

    // needed to next reload of App
    motor_config.position_nonce = position_for_onboarding_calc;
    motor_config.position = position_for_onboarding_calc;

    if (state.current_position < 0)
    {
        position_for_onboarding_calc = items.size() * 10000 + state.current_position;
    }

    current_onboarding_position = position_for_onboarding_calc % items.size();

    return EntityStateUpdate{};
}

void OnboardingApp::updateStateFromSystem(AppState state) {}

uint8_t OnboardingApp::navigationNext()
{
    return current_onboarding_position + 1; // +1 to shift from 0 position which is menu itself
}

TFT_eSprite *OnboardingApp::render()
{
    OnboardingItem current_item = find_item(current_onboarding_position);
    OnboardingItem prev_item;
    OnboardingItem next_item;
    if (current_onboarding_position == 0)
    {
        // prev_item = find_item(items.size() - 1);
        next_item = find_item(current_onboarding_position + 1);
    }
    else if (current_onboarding_position == items.size() - 1)
    {
        prev_item = find_item(current_onboarding_position - 1);
        // next_item = find_item(0);
    }
    else
    {
        prev_item = find_item(current_onboarding_position - 1);
        next_item = find_item(current_onboarding_position + 1);
    }
    render_onboarding_screen(current_item, prev_item, next_item);
    return this->spr_;
}

void OnboardingApp::add_item(uint8_t id, OnboardingItem item)
{
    items[id] = item;
    onboarding_items_count++;
    motor_config.max_position = onboarding_items_count;
}

// TODO: add protection, could cause panic
OnboardingItem OnboardingApp::find_item(uint8_t id)
{
    return (*items.find(id)).second;
}

void OnboardingApp::render_onboarding_screen(OnboardingItem current, OnboardingItem prev, OnboardingItem next)
{
    uint32_t color_active = current.color;
    uint32_t color_inactive = spr_->color565(150, 150, 150);
    uint32_t label_color = color_inactive;
    uint32_t background = spr_->color565(0, 0, 0);

    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_w = TFT_WIDTH / 2;

    uint16_t screen_radius = TFT_WIDTH / 2;

    // int8_t screen_name_label_w = 100;
    int8_t screen_name_label_h = spr_->fontHeight(1);
    int8_t label_vertical_offset = 25;

    int8_t call_to_action_label_h = spr_->fontHeight(1);

    uint8_t icon_size_big = 80; // TODO MAKE BIGGER
    uint8_t icon_size_small = 80;

    spr_->setTextDatum(CC_DATUM);
    spr_->setTextSize(1);
    spr_->setFreeFont(&NDS1210pt7b);

    // spr_->fillRect(center_h - room_lable_w / 2, label_vertical_offset, room_lable_w, room_lable_h + 1, label_color); // +1 for height to draw circle right
    // spr_->fillCircle(center_h - room_lable_w / 2, label_vertical_offset + room_lable_h / 2, room_lable_h / 2, label_color);
    // spr_->fillCircle(center_h + room_lable_w / 2, label_vertical_offset + room_lable_h / 2, room_lable_h / 2, label_color);

    if (current.big_icon == nullptr)
    {
        if (current.small_icon == nullptr)
        {
            spr_->setTextColor(color_active);
            spr_->drawString(current.screen_name, center_w, center_h - screen_name_label_h * 2, 1);
            spr_->drawString(current.screen_description, center_w, center_h - screen_name_label_h, 1);

            spr_->setTextColor(spr_->color565(128, 255, 80));
            spr_->drawString(current.call_to_action, center_w, TFT_WIDTH - (40 + call_to_action_label_h), 1);
        }
        else
        {
            spr_->setTextColor(color_active);
            spr_->drawString(current.screen_name, center_w, screen_name_label_h * 2, 1);
            spr_->drawString(current.screen_description, center_w, screen_name_label_h * 3, 1);

            spr_->drawBitmap(center_w - icon_size_big / 2, center_h - icon_size_big / 2 + 6, current.small_icon, icon_size_big, icon_size_big, current.color_small_icon, background);

            spr_->setTextColor(spr_->color565(128, 255, 80));
            spr_->drawString(current.call_to_action, center_w, TFT_WIDTH - (40 + call_to_action_label_h), 1);
        }
    }

    uint32_t menu_item_color;
    uint8_t menu_item_diameter = 6;
    uint8_t position_circle_radius = screen_radius - menu_item_diameter; // the radius of the circle where you want the dots to lay.
    float degree_per_item = 8 * PI / 180;                                // the degree (angle) between two points in radian
    float center_point_degree = 270 * PI / 180;                          //
    float menu_starting_angle = center_point_degree - ((onboarding_items_count * degree_per_item) / 2);

    for (uint16_t i = 0; i < onboarding_items_count; i++)
    {
        // draw a circle
        if (current_onboarding_position == i)
        {
            menu_item_color = TFT_GREENYELLOW;
        }
        else
        {
            menu_item_color = TFT_WHITE;
        }
        // polar coordinates
        spr_->fillCircle(screen_radius + (position_circle_radius * cosf(menu_starting_angle + degree_per_item * i)), screen_radius - position_circle_radius * sinf(menu_starting_angle + degree_per_item * i), menu_item_diameter / 2, menu_item_color);
    }

    // spr_->drawString(room, center_h, label_vertical_offset + room_lable_h / 2 - 1, 1);

    // spr_->drawBitmap(center_h - icon_size_active / 2, center_v - icon_size_active / 2, current.big_icon, icon_size_active, icon_size_active, color_active, background);
    // ESP_LOGD("menu.cpp", "%s", current.screen_name);

    // left one
    // spr_->drawBitmap(center_h - icon_size_active / 2 - 20 - icon_size_inactive, center_v - icon_size_inactive / 2, prev.small_icon, icon_size_inactive, icon_size_inactive, color_inactive, background);

    // right one
    // spr_->drawBitmap(center_h + icon_size_active / 2 + 20, center_v - icon_size_inactive / 2, next.small_icon, icon_size_inactive, icon_size_inactive, color_inactive, background);
};