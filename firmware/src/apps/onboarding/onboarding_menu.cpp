#include "./onboarding_menu.h"

OnboardingMenu::OnboardingMenu(TFT_eSprite *spr_) : Menu(spr_)
{
    back = UINT8_MAX;
    motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        0, // max position < min position indicates no bounds
        55 * PI / 180,
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

EntityStateUpdate OnboardingMenu::updateStateFromKnob(PB_SmartKnobState state)
{
    // TODO: cache menu size
    int32_t position_for_onboarding_calc = state.current_position;

    // needed to next reload of App
    motor_config.position_nonce = position_for_onboarding_calc;
    motor_config.position = position_for_onboarding_calc;

    set_menu_position(position_for_onboarding_calc);

    return EntityStateUpdate{};
}

void OnboardingMenu::updateStateFromSystem(AppState state) {}

void OnboardingMenu::add_item(int8_t id, std::shared_ptr<MenuItem> item)
{
    items[id] = item;
    motor_config.max_position = menu_items_count;
    menu_items_count++;
};

TFT_eSprite *OnboardingMenu::render()
{
    std::shared_ptr<MenuItem> item = find_item(get_menu_position());

    uint32_t background = spr_->color565(0, 0, 0);

    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_w = TFT_WIDTH / 2;

    uint16_t screen_radius = TFT_WIDTH / 2;

    int8_t screen_name_label_h = spr_->fontHeight(1);
    int8_t label_vertical_offset = 25;

    int8_t call_to_action_label_h = spr_->fontHeight(1);

    uint8_t icon_size_big = 80; // TODO MAKE BIGGER
    uint8_t icon_size_small = 80;

    spr_->setTextDatum(CC_DATUM);
    spr_->setTextSize(1);
    spr_->setFreeFont(&NDS1210pt7b);

    if (item->big_icon.icon == nullptr)
    {
        if (item->small_icon.icon == nullptr)
        {
            if (get_menu_position() == 3)
            {
                spr_->setTextColor(item->screen_name.color);
                spr_->drawString(item->screen_name.text, center_w, screen_name_label_h * 2, 1);

                spr_->setTextColor(item->screen_description.color);
                spr_->drawString(item->screen_description.text, center_w, screen_name_label_h * 3, 1);
            }
            else
            {
                spr_->setTextColor(item->screen_name.color);
                spr_->drawString(item->screen_name.text, center_w, center_h - screen_name_label_h * 2, 1);

                spr_->setTextColor(item->screen_description.color);
                spr_->drawString(item->screen_description.text, center_w, center_h - screen_name_label_h, 1);
            }

            spr_->setTextColor(item->call_to_action.color);
            spr_->drawString(item->call_to_action.text, center_w, TFT_WIDTH - (40 + call_to_action_label_h), 1);
        }
        else
        {
            spr_->setTextColor(item->screen_name.color);
            spr_->drawString(item->screen_name.text, center_w, screen_name_label_h * 2, 1);

            spr_->setTextColor(item->screen_description.color);
            spr_->drawString(item->screen_description.text, center_w, screen_name_label_h * 3, 1);

            spr_->drawBitmap(center_w - icon_size_big / 2, center_h - icon_size_big / 2 + 6, item->small_icon.icon, icon_size_big, icon_size_big, item->small_icon.color);

            spr_->setTextColor(item->call_to_action.color);
            spr_->drawString(item->call_to_action.text, center_w, TFT_WIDTH - (40 + call_to_action_label_h), 1);
        }
    }

    if (get_menu_position() == 3)
    {
        uint32_t colors[6] = {TFT_CYAN, TFT_PURPLE, TFT_RED, TFT_YELLOW, TFT_GREENYELLOW, TFT_GREEN};
        float angle_step = 2 * PI / 6;
        float circle_radius = 24; // Adjust as needed
        uint8_t small_circle_radius = 8;

        for (uint16_t i = 0; i < 6; i++)
        {
            int16_t x;
            int16_t y;
            x = center_w + circle_radius * sinf((i + 3) * angle_step);
            y = center_h + circle_radius * cosf((i + 3) * angle_step);

            spr_->fillCircle(x, y, small_circle_radius, colors[i]);
        }
    }

    uint8_t onboarding_items_count = get_menu_items_count();
    uint8_t current_onboarding_position = get_menu_position();

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

    return this->spr_;
}