#pragma once
#include "../network/spotify/spotify_api.h"
#include "../app.h"
#include "../util.h"

struct ProgressState
{
    unsigned long started_ms;
    unsigned long progress_ms;
    unsigned long song_duration_ms;

    lv_obj_t *progress = nullptr;
};

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
    lv_obj_t *player_screen = nullptr;
    lv_obj_t *album_img = nullptr;
    lv_obj_t *playing = nullptr;
    // lv_obj_t *progress = nullptr;
    lv_obj_t *track_name_label = nullptr;
    lv_obj_t *volume = nullptr;
    ProgressState progress_state_;
    lv_timer_t *progress_timer_ = nullptr;

    lv_obj_t *qr_screen = nullptr;
    lv_obj_t *qr_code = nullptr;

    bool first_run = true;
    uint8_t current_position = 0;
    int32_t last_position = 0;

    bool is_spotify_configured = false; // TODO Replace with actual check!!!!!!

    ConnectivityState last_connectivity_state;
    PlaybackState last_playback_state_;

    void updateTimer(const unsigned long &progress_ms, const unsigned long &song_duration_ms);

    QueueHandle_t shared_events_queue;
    void publishEvent(const WiFiEvent &event);
};