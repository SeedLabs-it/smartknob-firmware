#include "spotify.h"

SpotifyApp::SpotifyApp(SemaphoreHandle_t mutex, char *app_id_, char *friendly_name_, char *entity_id_) : App(mutex)
{
    sprintf(app_id, "%s", app_id_);
    sprintf(friendly_name, "%s", friendly_name_);
    sprintf(entity_id, "%s", entity_id_);

    motor_config = PB_SmartKnobConfig{
        .position = 0,
        .sub_position_unit = 0,
        .position_nonce = 0,
        .min_position = 0,
        .max_position = 100,
        .position_width_radians = 2.4 * PI / 180,
        .detent_strength_unit = 1,
        .endstop_strength_unit = 1,
        .snap_point = 1.1,
        .detent_positions_count = 0,
        .detent_positions = {},
        .snap_point_bias = 0,
        .led_hue = 27,
    };
    strncpy(motor_config.id, app_id, sizeof(motor_config.id) - 1);

    LV_IMG_DECLARE(x80_spotify);
    LV_IMG_DECLARE(x40_spotify);

    big_icon = x80_spotify;
    small_icon = x40_spotify;

    initScreen();
    initQrScreen();
}

void SpotifyApp::initScreen()
{
    SemaphoreGuard lock(mutex_);
    player_screen = lv_obj_create(screen);
    lv_obj_remove_style_all(player_screen);
    lv_obj_set_size(player_screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_add_flag(player_screen, LV_OBJ_FLAG_HIDDEN);

    album_img = lv_img_create(player_screen);
    lv_obj_align(album_img, LV_ALIGN_CENTER, 0, 0);

    track_name_label = lv_label_create(player_screen);
    lv_label_set_text(track_name_label, "No track playing");
    lv_obj_align(track_name_label, LV_ALIGN_CENTER, 0, 32);

    playback_state = lv_label_create(player_screen);
    lv_obj_set_style_text_font(playback_state, &lv_font_montserrat_30, 0); // TODO Add own symbol font!!
    lv_label_set_text(playback_state, LV_SYMBOL_PAUSE);
    lv_obj_center(playback_state);
}

void SpotifyApp::initQrScreen()
{
    SemaphoreGuard lock(mutex_);
    qr_screen = lv_obj_create(screen);
    lv_obj_remove_style_all(qr_screen);
    lv_obj_set_size(qr_screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_add_flag(qr_screen, LV_OBJ_FLAG_HIDDEN);

    qr_code = lv_qrcode_create(qr_screen, 80, LV_COLOR_MAKE(0x00, 0x00, 0x00), LV_COLOR_MAKE(0xFF, 0xFF, 0xFF));
    lv_qrcode_update(qr_code, "http://192.168.4.1/?spotify", strlen("http://192.168.4.1/?spotify"));
    lv_obj_align(qr_code, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *label = lv_label_create(qr_screen);
    lv_label_set_text(label, "Scan to configure Spotify");
    lv_obj_align_to(label, qr_code, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);
}

EntityStateUpdate SpotifyApp::updateStateFromKnob(PB_SmartKnobState state)
{
    EntityStateUpdate new_state;

    // LOGE("SpotifyApp::updateStateFromKnob");

    first_run = true;
    new_state.changed = false;
    return new_state;
}

void SpotifyApp::handleNavigation(NavigationEvent event)
{
    if (is_spotify_configured)
    {
        switch (event)
        {
        case SHORT:
            WiFiEvent wifi_event;

            if (last_playback_state_.is_playing)
            {
                wifi_event.type = SK_SPOTIFY_PAUSE;
            }
            else
            {
                wifi_event.type = SK_SPOTIFY_PLAY;
            }
            publishEvent(wifi_event);
            break;
        case LONG:
            break;
        default:
            break;
        }
    }
}

void SpotifyApp::updateStateFromHASS(MQTTStateUpdate mqtt_state_update) {}

void SpotifyApp::updateStateFromSystem(AppState state)
{
    if (shared_events_queue == nullptr && state.shared_events_queue != nullptr)
    {
        shared_events_queue = state.shared_events_queue;
    }

    if (state.connectivity_state.is_connected != last_connectivity_state.is_connected)
    {
        if (state.connectivity_state.is_connected)
        {
            static char ip_data[50];
            sprintf(ip_data, "http://%s/?spotify", state.connectivity_state.ip_address);
            lv_qrcode_update(qr_code, ip_data, strlen(ip_data));
        }
        else
        {
            {
                lv_qrcode_update(qr_code, "http://192.168.4.1/?spotify", strlen("http://192.168.4.1/?spotify"));
            }
        }
        last_connectivity_state = state.connectivity_state;
    }

    if (state.playback_state.spotify_available != last_playback_state_.spotify_available || state.playback_state.available != last_playback_state_.available || state.playback_state.timestamp != last_playback_state_.timestamp || state.playback_state.progress_ms != last_playback_state_.progress_ms || state.playback_state.is_playing != last_playback_state_.is_playing)
    {
        last_playback_state_ = state.playback_state;

        if (state.playback_state.is_playing)
        {
            lv_label_set_text(playback_state, LV_SYMBOL_PAUSE);
        }
        else
        {
            lv_label_set_text(playback_state, LV_SYMBOL_PLAY);
        }

        if (strcmp(state.playback_state.item.name, "") != 0)
        {
            lv_label_set_text(track_name_label, state.playback_state.item.name);
            lv_obj_align(track_name_label, LV_ALIGN_CENTER, 0, 32);
        }
        else
        {
            lv_label_set_text(track_name_label, "No track playing");
            lv_obj_align(track_name_label, LV_ALIGN_CENTER, 0, 32);
        }
    }

    if ((state.playback_state.spotify_available || state.playback_state.available) && lv_obj_has_flag(player_screen, LV_OBJ_FLAG_HIDDEN))
    {
        LOGE("Spotify is available");
        lv_obj_add_flag(qr_screen, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(player_screen, LV_OBJ_FLAG_HIDDEN);
        is_spotify_configured = true;
    }
    else if (!state.playback_state.spotify_available && lv_obj_has_flag(qr_screen, LV_OBJ_FLAG_HIDDEN))
    {
        LOGE("Spotify is not available");
        lv_obj_add_flag(player_screen, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(qr_screen, LV_OBJ_FLAG_HIDDEN);
    }
}

void SpotifyApp::publishEvent(const WiFiEvent &event)
{
    if (shared_events_queue != nullptr)
    {
        LOGE("Publishing event");
        xQueueSend(shared_events_queue, &event, 0);
    }
}