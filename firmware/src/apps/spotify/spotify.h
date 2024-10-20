#pragma once
#include "../network/spotify/spotify_api.h"
#include "../app.h"
#include "../util.h"

class SpotifyApp : public App
{
public:
    SpotifyApp(SemaphoreHandle_t mutex, char *app_id, char *friendly_name, char *entity_id);

    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void handleNavigation(NavigationEvent event);
    void updateStateFromHASS(MQTTStateUpdate mqtt_state_update);
    void updateStateFromSystem(AppState state);

protected:
    void initScreen();
    void initQrScreen();

private:
    SpotifyApi spotify;
    lv_obj_t *player_screen = nullptr;
    lv_obj_t *album_img = nullptr;
    lv_obj_t *playback_state = nullptr;
    lv_obj_t *volume = nullptr;

    lv_obj_t *qr_screen = nullptr;
    lv_obj_t *qr_code = nullptr;

    bool first_run = false;
    uint8_t current_position = 0;

    bool is_spotify_configured = false; // TODO Replace with actual check!!!!!!

    ConnectivityState last_connectivity_state;
};