#include "spotify_standalone.h"

SpotifyStandalone::SpotifyStandalone(SemaphoreHandle_t mutex) : Apps(mutex)
{
    loadApp(0, "spotify", "spotify", "Spotify", "spotify");
    setActive(0);
};
// void SpotifyStandalone::sync(cJSON *json_apps)
// {
//     clear();
//     uint16_t app_position = 0;

//     cJSON *json_app_ = NULL;
//     cJSON_ArrayForEach(json_app_, json_apps)
//     {

//         cJSON *json_app_slug = cJSON_GetObjectItemCaseSensitive(json_app_, "app_slug");
//         cJSON *json_app_id = cJSON_GetObjectItemCaseSensitive(json_app_, "app_id");
//         cJSON *json_entity_id = cJSON_GetObjectItemCaseSensitive(json_app_, "entity_id");
//         cJSON *json_friendly_name = cJSON_GetObjectItemCaseSensitive(json_app_, "friendly_name");

//         if (json_app_slug == NULL || json_app_id == NULL || json_entity_id == NULL || json_friendly_name == NULL)
//         {
//             LOGE("Invalid data.");
//             continue;
//         }

//         loadApp(app_position, json_app_slug->valuestring, json_app_id->valuestring, json_friendly_name->valuestring, json_entity_id->valuestring);

//         app_position++;
//     }

//     SettingsApp *settings_app = new SettingsApp(screen_mutex_);
//     settings_app->setOSConfigNotifier(os_config_notifier_);
//     add(app_position, settings_app);

//     updateMenu();
//     setMotorNotifier(motor_notifier);
//     // cJSON_Delete(json_apps); //DELETING DELETES POINTERS NEEDED TO DISPLAY FRIENDLY NAME ON APPS HMMMM
// }
void SpotifyStandalone::handleEvent(WiFiEvent event)
{
    // SemaphoreGuard lock(app_mutex_);
    // std::shared_ptr<App> app;

    // switch (event.type)
    // {
    // case SK_MQTT_STATE_UPDATE:
    //     if (event.body.mqtt_state_update.all == true)
    //     {
    //         for (auto &app : apps)
    //         {

    //             if (strcmp(app.second->app_id, event.body.mqtt_state_update.app_id) != 0 && strcmp(app.second->entity_id, event.body.mqtt_state_update.entity_id) == 0)
    //             {
    //                 app.second->updateStateFromHASS(event.body.mqtt_state_update);
    //             }
    //         }
    //     }
    //     else
    //     {
    //         app = find(event.body.mqtt_state_update.app_id);
    //         if (app != nullptr)
    //         {
    //             app->updateStateFromHASS(event.body.mqtt_state_update);
    //             motor_notifier->requestUpdate(active_app->getMotorConfig());
    //         }
    //         else
    //         {
    //             LOGW("App not found");
    //         }
    //     }
    //     break;
    // default:
    //     break;
    // }
}
void SpotifyStandalone::handleNavigationEvent(NavigationEvent event)
{
    // if (active_app == nullptr || apps.size() <= 1) // 1 is menu which doesnt get removed when sync = 0 apps
    // {
    //     return;
    // }
    if (event == NavigationEvent::SHORT)
    {
        Apps::handleNavigationEvent(event);
    }
}