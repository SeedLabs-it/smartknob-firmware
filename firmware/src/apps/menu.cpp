#include "menu.h"

MenuApp::MenuApp(TFT_eSprite *spr_) : App(spr_)
{
    sprintf(room, "%s", "Office");

    // mutex
    mutex = xSemaphoreCreateMutex();

    motor_config = PB_SmartKnobConfig{
        1,
        0,
        1,
        0,
        -1, // max position < min position indicates no bounds
        25 * PI / 180,
        2,
        1,
        0.55,
        "SKDEMO_Menu", // TODO: clean this
        0,
        {},
        0,
        20,
    };
}

EntityStateUpdate MenuApp::updateStateFromKnob(PB_SmartKnobState state)
{
    // TODO: cache menu size
    // might be needed to make sure first item is selected on "reload" but might fuck if user returns from app to menu again
    // motor_config.position_nonce = 1;
    // motor_config.position = 1;

    int32_t position_for_menu_calc = state.current_position;

    // needed to next reload of App
    motor_config.position_nonce = position_for_menu_calc;
    motor_config.position = position_for_menu_calc;

    if (state.current_position < 0)
    {
        position_for_menu_calc = items.size() * 10000 + state.current_position;
    }

    current_menu_position = position_for_menu_calc % items.size();

    return EntityStateUpdate{};
}

void MenuApp::updateStateFromSystem(AppState state) {}

std::pair<app_types, uint8_t> MenuApp::navigationNext()
{
    return std::make_pair(type, find_item(current_menu_position)->app_id);
}

TFT_eSprite *MenuApp::render()
{
    lock();
    current_item = find_item(current_menu_position);

    if (current_menu_position == 0)
    {
        prev_item = find_item(items.size() - 1);
        next_item = find_item(current_menu_position + 1);
    }
    else if (current_menu_position == items.size() - 1)
    {
        prev_item = find_item(current_menu_position - 1);
        next_item = find_item(0);
    }
    else
    {
        prev_item = find_item(current_menu_position - 1);
        next_item = find_item(current_menu_position + 1);
    }
    unlock();

    render_menu_screen();
    return this->spr_;
}

std::string MenuApp::getClassName()
{
    return "App";
}

// TODO: add protection, could cause panic
MenuItem *MenuApp::find_item(uint8_t id)
{
    return &(*items.find(id)).second;
}

// TODO: add protection of overwriting same items
void MenuApp::add_item(uint8_t id, MenuItem item)
{
    items.insert(std::make_pair(id, item));
}

void MenuApp::render_menu_screen()
{
    lock();
    uint32_t color_active = current_item->color;
    uint32_t color_inactive = spr_->color565(150, 150, 150);
    uint32_t label_color = color_inactive;
    uint32_t background = spr_->color565(0, 0, 0);

    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;

    int8_t room_lable_w = 100;
    int8_t room_lable_h = 30;
    int8_t label_vertical_offset = 25;

    uint8_t icon_size_active = 80;
    uint8_t icon_size_inactive = 40;

    spr_->setTextDatum(CC_DATUM);

    spr_->fillRect(center_h - room_lable_w / 2, label_vertical_offset, room_lable_w, room_lable_h + 1, label_color); // +1 for height to draw circle right
    spr_->fillCircle(center_h - room_lable_w / 2, label_vertical_offset + room_lable_h / 2, room_lable_h / 2, label_color);
    spr_->fillCircle(center_h + room_lable_w / 2, label_vertical_offset + room_lable_h / 2, room_lable_h / 2, label_color);

    spr_->setTextColor(TFT_BLACK);
    spr_->setFreeFont(&Roboto_Thin_Bold_24);
    spr_->drawString(room, center_h, label_vertical_offset + room_lable_h / 2 - 1, 1);

    spr_->drawBitmap(center_h - icon_size_active / 2, center_v - icon_size_active / 2, current_item->big_icon, icon_size_active, icon_size_active, color_active, background);

    // left one
    spr_->drawBitmap(center_h - icon_size_active / 2 - 20 - icon_size_inactive, center_v - icon_size_inactive / 2, prev_item->small_icon, icon_size_inactive, icon_size_inactive, color_inactive, background);

    // right one
    spr_->drawBitmap(center_h + icon_size_active / 2 + 20, center_v - icon_size_inactive / 2, next_item->small_icon, icon_size_inactive, icon_size_inactive, color_inactive, background);

    spr_->setTextColor(color_active);
    spr_->setFreeFont(&Roboto_Thin_24);
    spr_->drawString(current_item->screen_name, center_h, center_v + icon_size_active / 2 + 30, 1);
    unlock();
};

void MenuApp::lock()
{
    xSemaphoreTake(mutex, portMAX_DELAY);
}

void MenuApp::unlock()
{
    xSemaphoreGive(mutex);
}