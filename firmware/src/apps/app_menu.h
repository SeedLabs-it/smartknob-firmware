#pragma once
#include "menu.h"
#include "font/roboto_thin_bold_24.h"

#include <map>

const uint8_t SCREEN_NAME_LENGTH = 20;
const uint8_t MEX_MENU_ITEMS = 12;

class MenuApp : public Menu
{
public:
    MenuApp(SemaphoreHandle_t mutex) : Menu(mutex)
    {
        sprintf(this->app_id, "%s", "MENU");
        back_ = MENU;
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
            "MENU",
            0,
            {},
            0,
            20,
        };
    }

    // TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state)
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
            position_for_menu_calc = get_menu_page_count() * 10000 + state.current_position;
        }

        uint8_t current_menu_position = position_for_menu_calc % get_menu_page_count();
        uint8_t old_menu_position = get_menu_position();

        if (current_menu_position == old_menu_position)
        {
            return EntityStateUpdate{};
        }

        set_menu_position(current_menu_position);

        if (current_menu_position == 0)
        {
            prev_page = find_page(get_menu_page_count() - 1);
            next_page = find_page(current_menu_position + 1);
        }
        else if (current_menu_position == get_menu_page_count() - 1)
        {
            prev_page = find_page(current_menu_position - 1);
            next_page = find_page(0);
        }
        else
        {
            prev_page = find_page(current_menu_position - 1);
            next_page = find_page(current_menu_position + 1);
        }

        next_ = find_page(current_menu_position)->app_id_;
        {
            SemaphoreGuard lock(mutex_);
            find_page(old_menu_position)->hide();
            find_page(current_menu_position)->show();

            lv_image_set_src(left_image_icon, prev_page->getSmallIcon());
            lv_image_set_src(right_image_icon, next_page->getSmallIcon());
        }

        return EntityStateUpdate{};
    }
    void updateStateFromSystem(AppState state) {};

    // void add_item(int8_t id, std::shared_ptr<MenuItem> item)
    // {
    //     items[id] = item;
    //     menu_page_count++;
    // };
    void update();

    void render()
    {
        App::render();
        find_page(get_menu_position())->show();
    }

private:
    char room[12];

    std::shared_ptr<MenuPage> current_page;
    std::shared_ptr<MenuPage> prev_page;
    std::shared_ptr<MenuPage> next_page;
};