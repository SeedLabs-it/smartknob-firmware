#pragma once

#include "app.h"
#include "font/NDS1210pt7b.h"

#include <map>

struct TextItem
{
    const char *text;
    uint16_t color;
};

struct IconItem
{
    const unsigned char *icon;
    uint16_t color;
};
struct OnboardingItem
{
    uint16_t app_id;
    TextItem screen_name;
    TextItem screen_description;
    TextItem call_to_action;
    IconItem big_icon;
    IconItem small_icon;
};

class OnboardingApp : public App
{

public:
    const app_types type = menu_type;
    OnboardingApp(TFT_eSprite *spr_);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);
    std::pair<app_types, uint8_t> navigationNext();
    void add_item(uint8_t id, OnboardingItem item);
    OnboardingItem find_item(uint8_t id);

private:
    std::map<uint8_t, OnboardingItem> items;
    uint8_t onboarding_items_count = 0;
    uint8_t current_onboarding_position = 0;

    void render_onboarding_screen(OnboardingItem current);
};
