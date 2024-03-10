#include "hass_apps.h"

void HassApps::sync(cJSON *json_apps)
{
    clear();
    uint16_t app_position = 0;

    cJSON *json_app_ = NULL;
    cJSON_ArrayForEach(json_app_, json_apps)
    {

        cJSON *json_app_slug = cJSON_GetObjectItemCaseSensitive(json_app_, "app_slug");
        cJSON *json_app_id = cJSON_GetObjectItemCaseSensitive(json_app_, "app_id");
        cJSON *json_friendly_name = cJSON_GetObjectItemCaseSensitive(json_app_, "friendly_name");

        loadApp(app_position, json_app_slug->valuestring, json_app_id->valuestring, json_friendly_name->valuestring);

        app_position++;
    }

    SettingsApp *settings_app = new SettingsApp(this->spr_);
    add(app_position, settings_app);

    updateMenu();
    // cJSON_Delete(json_apps); //DELETING DELETES POINTERS NEEDED TO DISPLAY FRIENDLY NAME ON APPS HMMMM
}

void HassApps::handleEvent(WiFiEvent event)
{
    lock();
    std::shared_ptr<App> app;

    switch (event.type)
    {
    case SK_MQTT_STATE_UPDATE:
        app = find(event.body.mqtt_state_update.app_id);
        if (app != nullptr)
        {
            app->updateStateFromHASS(event.body.mqtt_state_update);
            motor_notifier->requestUpdate(active_app->getMotorConfig());
        }
        else
        {
            ESP_LOGD("HASS_APPS", "APP NOT FOUND");
        }

        // cJSON_Delete(event.body.mqtt_state_update.state);
        break;
    default:
        break;
    }
    unlock();
}