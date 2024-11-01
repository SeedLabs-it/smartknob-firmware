#include "spotify.h"

void progress_timer(lv_timer_t *progress_timer)
{
    ProgressState *user_data = (ProgressState *)progress_timer->user_data;
    unsigned long started_ms = user_data->started_ms;
    unsigned long progress_ms = user_data->progress_ms + (millis() - started_ms);
    unsigned long song_duration_ms = user_data->song_duration_ms;

    lv_arc_set_angles(user_data->progress, 0, 260 * (1 - (((song_duration_ms - progress_ms) / (float)song_duration_ms))));
    if (progress_ms > song_duration_ms)
    {
        lv_arc_set_value(user_data->progress, 100);
        return;
    }
}

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
        .max_position = 20,
        .position_width_radians = 6 * PI / 180,
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
    lv_obj_set_size(album_img, 300, 300);
    lv_obj_center(album_img);
    lv_obj_set_style_img_opa(album_img, LV_OPA_60, 0);

    // lv_obj_t *opacity_cover = lv_obj_create(player_screen);
    // lv_obj_set_size(opacity_cover, 300, 300);
    // lv_obj_align(album_img, LV_ALIGN_CENTER, 0, 0);
    // lv_obj_set_style_bg_color(opacity_cover, LV_COLOR_MAKE(0, 0, 0), 0);
    // lv_obj_set_style_bg_opa(opacity_cover, LV_OPA_50, 0);

    track_name_label = lv_label_create(player_screen);
    lv_label_set_text(track_name_label, "No track playing");
    lv_obj_align(track_name_label, LV_ALIGN_CENTER, 0, 32);

    playing = lv_label_create(player_screen);
    lv_obj_set_style_text_font(playing, &lv_font_montserrat_30, 0); // TODO Add own symbol font!!
    lv_label_set_text(playing, LV_SYMBOL_PAUSE);
    lv_obj_center(playing);

    progress_state_.progress = lv_arc_create(player_screen);
    lv_obj_t *progress = progress_state_.progress;
    lv_obj_set_size(progress, 226, 226);
    lv_obj_set_style_arc_color(progress, LV_COLOR_MAKE(0x1D, 0xB9, 0x54), LV_PART_INDICATOR);

    lv_obj_set_style_arc_width(progress, 4, LV_PART_MAIN);
    lv_obj_set_style_arc_width(progress, 4, LV_PART_INDICATOR);

    lv_arc_set_rotation(progress, 140);
    lv_arc_set_bg_angles(progress, 0, 260);
    lv_arc_set_angles(progress, 0, 0);
    lv_obj_remove_style(progress, NULL, LV_PART_KNOB);
    lv_obj_center(progress);

    volume = lv_arc_create(player_screen);
    lv_obj_set_size(volume, 226, 226);
    lv_obj_set_style_arc_color(volume, LV_COLOR_MAKE(0xBB, 0xBB, 0xBB), LV_PART_INDICATOR);

    lv_obj_set_style_arc_width(volume, 4, LV_PART_MAIN);
    lv_obj_set_style_arc_width(volume, 4, LV_PART_INDICATOR);

    lv_arc_set_rotation(volume, 50);
    lv_arc_set_bg_angles(volume, 0, 80);
    lv_arc_set_angles(volume, 0, 0);
    lv_obj_remove_style(volume, NULL, LV_PART_KNOB);
    lv_obj_center(volume);
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

    current_position = state.current_position;

    motor_config.position_nonce = current_position;
    motor_config.position = current_position;

    if (last_position != current_position && !first_run)
    {
        lv_arc_set_value(volume, state.current_position * 5);

        WiFiEvent wifi_event;
        wifi_event.type = SK_SPOTIFY_VOLUME;
        wifi_event.body.volume = state.current_position * 5;
        publishEvent(wifi_event);
    }

    first_run = false;
    last_position = current_position;
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
    if (shared_events_queue == nullptr && state.shared_events_queue != nullptr || shared_events_queue != state.shared_events_queue)
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

    if (state.cover_art != nullptr && latest_cover_art != state.cover_art)
    {
        LOGE("COVER ART");
        latest_cover_art = state.cover_art;
        lv_img_set_src(album_img, latest_cover_art);
    }

    if (state.playback_state.spotify_available != last_playback_state_.spotify_available || state.playback_state.available != last_playback_state_.available || state.playback_state.timestamp != last_playback_state_.timestamp || state.playback_state.progress_ms != last_playback_state_.progress_ms || state.playback_state.is_playing != last_playback_state_.is_playing)
    {
        if (state.playback_state.is_playing)
        {
            // updateTimer(state.playback_state.progress_ms, state.playback_state.item.duration_ms);
            if (progress_timer_ != nullptr)
            {
                progress_state_.started_ms = millis();
                lv_timer_resume(progress_timer_);
            }
            lv_label_set_text(playing, LV_SYMBOL_PAUSE);
        }
        else
        {
            // updateTimer(state.playback_state.progress_ms, state.playback_state.item.duration_ms);
            if (progress_timer_ != nullptr)
            {
                lv_timer_pause(progress_timer_);
                progress_state_.progress_ms = progress_state_.progress_ms + (millis() - progress_state_.started_ms);
            }
            lv_label_set_text(playing, LV_SYMBOL_PLAY);
        }

        // LOGE("POINTER %p", state.playback_state.image_dsc);

        // if (state.playback_state.image_dsc != nullptr)
        // {
        //     if (last_playback_state_.image_dsc == nullptr)
        //         LOGE("LAST IMGAGE NULL");
        //     if (last_playback_state_.image_dsc == nullptr || state.playback_state.image_dsc->data != last_playback_state_.image_dsc->data)
        //     {
        //         LOGE("Setting image");
        //         lv_img_set_src(album_img, state.playback_state.image_dsc);
        //     }
        // }

        if (state.playback_state.device.volume_percent != last_playback_state_.device.volume_percent)
        {
            current_position = state.playback_state.device.volume_percent / 5;
            last_position = current_position;
            motor_config.position_nonce = current_position;
            motor_config.position = current_position;
            lv_arc_set_value(volume, state.playback_state.device.volume_percent);

            motor_notifier->requestUpdate(motor_config);
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

            lv_img_set_src(album_img, NULL);

            lv_arc_set_value(progress_state_.progress, 0);

            lv_label_set_text(playing, LV_SYMBOL_PLAY);
        }

        if (strcmp(state.playback_state.item.name, last_playback_state_.item.name) != 0 || state.playback_state.progress_ms != last_playback_state_.progress_ms)
        {
            updateTimer(state.playback_state.progress_ms, state.playback_state.item.duration_ms);
            if (!state.playback_state.is_playing)
            {
                lv_timer_pause(progress_timer_);
            }
        }

        last_playback_state_ = state.playback_state;
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

void SpotifyApp::updateTimer(const unsigned long &progress_ms, const unsigned long &song_duration_ms)
{
    if (progress_timer_ != nullptr)
    {
        lv_timer_del(progress_timer_);
    }
    progress_state_.started_ms = millis();
    progress_state_.progress_ms = progress_ms;
    progress_state_.song_duration_ms = song_duration_ms;

    progress_timer_ = lv_timer_create(progress_timer, 500, &progress_state_);
}

void SpotifyApp::publishEvent(const WiFiEvent &event)
{
    if (shared_events_queue != nullptr)
    {
        xQueueSend(shared_events_queue, &event, 0);
    }
}