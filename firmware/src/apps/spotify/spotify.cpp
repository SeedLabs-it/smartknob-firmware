#include "spotify.h"

SpotifyApp::SpotifyApp(SemaphoreHandle_t mutex, char *app_id_, char *friendly_name_, char *entity_id_) : App(mutex)
{
    sprintf(app_id, "%s", app_id_);
    sprintf(friendly_name, "%s", friendly_name_);
    sprintf(entity_id, "%s", entity_id_);

    motor_config = PB_SmartKnobConfig{
        current_position,
        0,
        current_position,
        0,
        1,
        60 * PI / 180,
        1,
        1,
        0.55, // Note the snap point is slightly past the midpoint (0.5); compare to normal detents which use a snap point *past* the next value (i.e. > 1)
        "",
        0,
        {},
        0,
        27,
    };
    strncpy(motor_config.id, app_id, sizeof(motor_config.id) - 1);

    LV_IMG_DECLARE(x80_lightbulb_outline);
    LV_IMG_DECLARE(x40_lightbulb_outline);

    big_icon = x80_lightbulb_outline;
    small_icon = x40_lightbulb_outline;

    // initScreen();
    if (is_spotify_configured)
    {
        initScreen();
        lv_obj_clear_flag(player_screen, LV_OBJ_FLAG_HIDDEN);
    }
    else
    {
        initQrScreen();
        lv_obj_clear_flag(qr_screen, LV_OBJ_FLAG_HIDDEN);
    }
}

void SpotifyApp::initScreen()
{
    SemaphoreGuard lock(mutex_);
    player_screen = lv_obj_create(screen);
    lv_obj_remove_style_all(player_screen);
    lv_obj_set_size(player_screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_add_flag(player_screen, LV_OBJ_FLAG_HIDDEN);

    pause_icon = lv_label_create(player_screen);
    lv_label_set_text(pause_icon, LV_SYMBOL_PAUSE);
    lv_obj_align(pause_icon, LV_ALIGN_CENTER, 0, 0);
}

void SpotifyApp::initQrScreen()
{
    SemaphoreGuard lock(mutex_);
    qr_screen = lv_obj_create(screen);
    lv_obj_remove_style_all(qr_screen);
    lv_obj_set_size(qr_screen, LV_HOR_RES, LV_VER_RES);
    lv_obj_add_flag(qr_screen, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *qr_code = lv_qrcode_create(qr_screen, 80, LV_COLOR_MAKE(0x00, 0x00, 0x00), LV_COLOR_MAKE(0xFF, 0xFF, 0xFF));
    lv_qrcode_update(qr_code, "http://192.168.4.1/spotify", strlen("http://192.168.4.1/spotify"));
    lv_obj_align(qr_code, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *label = lv_label_create(qr_screen);
    lv_label_set_text(label, "Scan to configure Spotify");
    lv_obj_align_to(label, qr_code, LV_ALIGN_OUT_BOTTOM_MID, 0, 6);
}

EntityStateUpdate SpotifyApp::updateStateFromKnob(PB_SmartKnobState state)
{
    EntityStateUpdate new_state;

    first_run = true;
    return new_state;
}

void SpotifyApp::updateStateFromHASS(MQTTStateUpdate mqtt_state_update) {}

void SpotifyApp::updateStateFromSystem(AppState state) {}