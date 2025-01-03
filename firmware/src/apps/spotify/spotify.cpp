#include "spotify.h"

void progress_timer(lv_timer_t *progress_timer)
{
    ProgressState *user_data = (ProgressState *)progress_timer->user_data;
    unsigned long started_ms = user_data->started_ms;
    unsigned long progress_ms = user_data->progress_ms + (millis() - started_ms);
    unsigned long song_duration_ms = user_data->song_duration_ms;

    float end_angle = SPOTIFY_ARC_MAX_ANGLE * (1 - (((song_duration_ms - progress_ms) / (float)song_duration_ms)));
    if (end_angle < 5)
        end_angle = 5;

    if (progress_ms < song_duration_ms)
    {
        lv_arc_set_angles(user_data->progress, end_angle - 5, end_angle);
    }
    else
    {
        lv_arc_set_angles(user_data->progress, SPOTIFY_ARC_MAX_ANGLE - 5, SPOTIFY_ARC_MAX_ANGLE);
        return;
    }
}

void hide_volume_arc(void *volume_arc)
{
    LOGE("HIDE VOLUME ARC");
    delay(3000);
    LOGE("HIDE VOLUME ARC END");
}

SpotifyApp::SpotifyApp(SemaphoreHandle_t mutex, char *app_id_, char *friendly_name_, char *entity_id_) : App(mutex)
{
    sprintf(app_id, "%s", app_id_);
    sprintf(friendly_name, "%s", friendly_name_);
    sprintf(entity_id, "%s", entity_id_);

    motor_config = blocked_motor_config;
    strncpy(motor_config.id, app_id, sizeof(motor_config.id) - 1);
    strncpy(spotify_config_.id, app_id, sizeof(spotify_config_.id) - 1);

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
    lv_obj_set_size(album_img, 240, 240);
    lv_obj_center(album_img);
    // lv_obj_set_style_img_opa(album_img, LV_OPA_60, 0);

    progress_state_.progress = lv_arc_create(player_screen);
    lv_obj_t *progress = progress_state_.progress;
    lv_obj_set_size(progress, 232, 232);
    lv_obj_set_style_arc_color(progress, LV_COLOR_MAKE(0x99, 0x99, 0x99), LV_PART_MAIN);
    lv_obj_set_style_arc_color(progress, LV_COLOR_MAKE(0x1D, 0xB9, 0x54), LV_PART_INDICATOR);

    lv_obj_set_style_arc_width(progress, 4, LV_PART_MAIN);
    lv_obj_set_style_arc_width(progress, 4, LV_PART_INDICATOR);

    lv_obj_set_style_arc_rounded(progress, false, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(progress, false, LV_PART_INDICATOR);

    lv_arc_set_rotation(progress, 271);
    lv_arc_set_bg_angles(progress, 0, SPOTIFY_ARC_MAX_ANGLE);
    lv_arc_set_angles(progress, 0, 0);
    lv_obj_remove_style(progress, NULL, LV_PART_KNOB);
    lv_obj_center(progress);

    lv_obj_set_style_radius(progress, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(progress, 0, LV_PART_INDICATOR);

    volume = lv_arc_create(player_screen);
    lv_obj_set_size(volume, 219, 219);
    lv_obj_set_style_arc_color(volume, LV_COLOR_MAKE(0x99, 0x99, 0x99), LV_PART_MAIN);
    lv_obj_set_style_arc_color(volume, LV_COLOR_MAKE(0x1D, 0xB9, 0x54), LV_PART_INDICATOR);

    lv_obj_set_style_arc_width(volume, 4, LV_PART_MAIN);
    lv_obj_set_style_arc_width(volume, 4, LV_PART_INDICATOR);

    lv_obj_set_style_arc_rounded(volume, false, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(volume, false, LV_PART_INDICATOR);

    lv_arc_set_rotation(volume, 140);
    lv_arc_set_bg_angles(volume, 0, 260);
    lv_arc_set_angles(volume, 0, 0);
    lv_obj_remove_style(volume, NULL, LV_PART_KNOB);
    lv_obj_center(volume);

    playing = lv_label_create(player_screen);
    lv_obj_set_style_text_font(playing, &lv_font_montserrat_30, 0); // TODO Add own symbol font!!
    lv_obj_set_style_text_color(playing, LV_COLOR_MAKE(0x00, 0x00, 0x00), 0);
    lv_obj_set_style_opa(playing, LV_OPA_70, 0);
    lv_label_set_text(playing, LV_SYMBOL_PAUSE);
    lv_obj_center(playing);

    lv_obj_t *text_box = lv_obj_create(player_screen);
    lv_obj_remove_style_all(text_box);
    lv_obj_set_size(text_box, 240, 50);
    lv_obj_align(text_box, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_obj_set_flex_flow(text_box, LV_FLEX_FLOW_COLUMN);
    // // lv_obj_set_flex_align(text_box, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);  // Left align text
    lv_obj_set_flex_align(text_box, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER); // Center text

    lv_obj_set_style_pad_row(text_box, 3, 0);

    track_name_label = lv_label_create(text_box);
    lv_label_set_text(track_name_label, "No track playing");
    lv_label_set_long_mode(track_name_label, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_font(track_name_label, &aktivgrotesk_regular_10pt, 0);
    // lv_obj_align(track_name_label, LV_ALIGN_BOTTOM_MID, 0, -28);

    track_artist_label = lv_label_create(text_box);
    lv_label_set_text(track_artist_label, "");
    lv_label_set_long_mode(track_artist_label, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_font(track_artist_label, &aktivgrotesk_regular_10pt, 0);
    // lv_obj_align_to(track_artist_label, track_name_label, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);
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
        // last_updated_ms_ = millis();
        // lv_obj_clear_flag(volume, LV_OBJ_FLAG_HIDDEN);
        lv_arc_set_value(volume, state.current_position);

        WiFiEvent wifi_event;
        wifi_event.type = SK_SPOTIFY_VOLUME;
        wifi_event.body.volume = state.current_position;
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

    if ((state.os_mode_state == OSMode::SPOTIFY || state.playback_state.spotify_available || state.playback_state.available) && lv_obj_has_flag(player_screen, LV_OBJ_FLAG_HIDDEN))
    {
        LOGV(LOG_LEVEL_DEBUG, "Spotify is available");
        lv_obj_add_flag(qr_screen, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(player_screen, LV_OBJ_FLAG_HIDDEN);
        motor_config = spotify_config_;
        LOGE("ENABLING SPOTIFY MOTOR CONFIG");
        is_spotify_configured = true;
    }
    else if (state.os_mode_state != OSMode::SPOTIFY && !state.playback_state.spotify_available && lv_obj_has_flag(qr_screen, LV_OBJ_FLAG_HIDDEN))
    {
        LOGW("Spotify is not available");
        lv_obj_add_flag(player_screen, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(qr_screen, LV_OBJ_FLAG_HIDDEN);
        motor_config = blocked_motor_config;
        LOGE("DISABLING SPOTIFY MOTOR CONFIG");
        is_spotify_configured = false;
    }

    if (state.cover_art != nullptr && latest_cover_art != state.cover_art) // TODO Cover art should arrive after colors but technically it could arrive before
    {
        lv_img_cache_invalidate_src(latest_cover_art);
        heap_caps_free(latest_cover_art);

        latest_cover_art = state.cover_art;

        lv_img_set_src(album_img, state.cover_art);

        if (state.cover_art_colors != nullptr && latest_cover_art_colors != state.cover_art_colors)
        {
            if (latest_cover_art_colors != nullptr)
                heap_caps_free(latest_cover_art_colors);

            latest_cover_art_colors = state.cover_art_colors;

            lv_obj_set_style_arc_color(progress_state_.progress, latest_cover_art_colors[0], LV_PART_INDICATOR);
            lv_obj_set_style_arc_color(volume, latest_cover_art_colors[0], LV_PART_INDICATOR);

            lv_obj_set_style_arc_color(progress_state_.progress, latest_cover_art_colors[1], LV_PART_MAIN);
            lv_obj_set_style_arc_color(volume, latest_cover_art_colors[1], LV_PART_MAIN);
        }
    }

    if (state.playback_state.spotify_available != last_playback_state_.spotify_available || state.playback_state.available != last_playback_state_.available || state.playback_state.timestamp != last_playback_state_.timestamp || state.playback_state.progress_ms != last_playback_state_.progress_ms || state.playback_state.is_playing != last_playback_state_.is_playing)
    {
        if (state.playback_state.is_playing)
        {
            if (progress_timer_ != nullptr)
            {
                progress_state_.started_ms = millis();
                lv_timer_resume(progress_timer_);
            }
            lv_label_set_text(playing, LV_SYMBOL_PAUSE);
        }
        else
        {
            if (progress_timer_ != nullptr)
            {
                lv_timer_pause(progress_timer_);
                progress_state_.progress_ms = progress_state_.progress_ms + (millis() - progress_state_.started_ms);
            }
            lv_label_set_text(playing, LV_SYMBOL_PLAY);
        }

        if (state.playback_state.device.volume_percent != last_playback_state_.device.volume_percent)
        {
            current_position = state.playback_state.device.volume_percent;
            last_position = current_position;
            motor_config.position_nonce = current_position;
            motor_config.position = current_position;
            lv_arc_set_value(volume, state.playback_state.device.volume_percent);

            motor_notifier->requestUpdate(motor_config);
        }

        if (strcmp(state.playback_state.item.name, "") != 0)
        {
            lv_label_set_text(track_name_label, state.playback_state.item.name);
            lv_coord_t width_track_name = lv_txt_get_width(
                state.playback_state.item.name,
                strlen(state.playback_state.item.name),
                &aktivgrotesk_regular_10pt,
                0,
                LV_TEXT_FLAG_NONE);

            if (width_track_name > 130)
            {
                lv_obj_set_size(track_name_label, 130, 11); // 11 is the height of the font
            }
            else
            {
                lv_obj_set_size(track_name_label, width_track_name, 11); // 11 is the height of the font
            }

            lv_coord_t width_track_artist = lv_txt_get_width(
                state.playback_state.item.artist,
                strlen(state.playback_state.item.artist),
                &aktivgrotesk_regular_10pt,
                0,
                LV_TEXT_FLAG_NONE);

            if (width_track_artist > 90)
            {
                lv_obj_set_size(track_artist_label, 90, 11); // 11 is the height of the font
            }
            else
            {
                lv_obj_set_size(track_artist_label, width_track_artist, 11); // 11 is the height of the font
            }
        }
        else
        {
            lv_label_set_text(track_name_label, "No track playing");

            lv_img_set_src(album_img, NULL);

            lv_arc_set_value(progress_state_.progress, 0);

            lv_label_set_text(playing, LV_SYMBOL_PLAY);
        }

        if (strcmp(state.playback_state.item.artist, "") != 0)
        {
            lv_label_set_text(track_artist_label, state.playback_state.item.artist);
        }
        else
        {
            lv_label_set_text(track_artist_label, "");
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