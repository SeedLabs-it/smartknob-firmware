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
    if (current_onboarding_position >= 1 && current_onboarding_position <= 3)
        return current_onboarding_position;

    return 0; // +1 to shift from 0 position which is menu itself
}

TFT_eSprite *OnboardingApp::render()
{
    OnboardingItem current_item = find_item(current_onboarding_position);
    render_onboarding_screen(current_item);
    return this->spr_;
}

void OnboardingApp::add_item(uint8_t id, OnboardingItem item)
{
    items[id] = item;
    onboarding_items_count++;
    motor_config.max_position = onboarding_items_count - 1;
}

// TODO: add protection, could cause panic
OnboardingItem OnboardingApp::find_item(uint8_t id)
{
    return (*items.find(id)).second;
}

void OnboardingApp::render_onboarding_screen(OnboardingItem item)
{
    uint32_t color_active = item.screen_name.color;
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

    if (item.big_icon.icon == nullptr)
    {
        if (item.small_icon.icon == nullptr)
        {
            spr_->setTextColor(color_active);
            spr_->drawString(item.screen_name.text, center_w, center_h - screen_name_label_h * 2, 1);
            spr_->drawString(item.screen_description.text, center_w, center_h - screen_name_label_h, 1);

            spr_->setTextColor(item.call_to_action.color);
            spr_->drawString(item.call_to_action.text, center_w, TFT_WIDTH - (40 + call_to_action_label_h), 1);
        }
        else
        {
            spr_->setTextColor(color_active);
            spr_->drawString(item.screen_name.text, center_w, screen_name_label_h * 2, 1);
            spr_->drawString(item.screen_description.text, center_w, screen_name_label_h * 3, 1);

            spr_->drawBitmap(center_w - icon_size_big / 2, center_h - icon_size_big / 2 + 6, item.small_icon.icon, icon_size_big, icon_size_big, item.small_icon.color, background);

            spr_->setTextColor(item.call_to_action.color);
            spr_->drawString(item.call_to_action.text, center_w, TFT_WIDTH - (40 + call_to_action_label_h), 1);
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
};