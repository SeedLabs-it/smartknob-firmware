#pragma once
#include "../app.h"
#include "../util.h"

class SpotifyApp : public App
{
public:
    SpotifyApp(SemaphoreHandle_t mutex, char *app_id, char *friendly_name, char *entity_id);
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromHASS(MQTTStateUpdate mqtt_state_update);
    void updateStateFromSystem(AppState state);

protected:
    void initScreen();
    void initQrScreen();

private:
    lv_obj_t *player_screen = nullptr;
    lv_obj_t *pause_icon = nullptr;
    lv_obj_t *volume = nullptr;

    lv_obj_t *qr_screen = nullptr;

    // lv_obj_t *cover_art = nullptr;

    bool first_run = false;
    uint8_t current_position = 0;

    bool is_spotify_configured = false; // TODO Replace with actual check!!!!!!
};