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
        cJSON *json_entity_id = cJSON_GetObjectItemCaseSensitive(json_app_, "entity_id");
        cJSON *json_friendly_name = cJSON_GetObjectItemCaseSensitive(json_app_, "friendly_name");

        loadApp(app_position, json_app_slug->valuestring, json_app_id->valuestring, json_friendly_name->valuestring, json_entity_id->valuestring);

        app_position++;
    }

    SettingsApp *settings_app = new SettingsApp(this->spr_);
    add(app_position, settings_app);

    updateMenu();
    setMotorNotifier(motor_notifier);
    // cJSON_Delete(json_apps); //DELETING DELETES POINTERS NEEDED TO DISPLAY FRIENDLY NAME ON APPS HMMMM
}

void HassApps::handleEvent(WiFiEvent event)
{
    lock();
    std::shared_ptr<App> app;

    switch (event.type)
    {
    case SK_MQTT_STATE_UPDATE:
        if (event.body.mqtt_state_update.all == true)
        {
            for (auto &app : apps)
            {

                if (strcmp(app.second->app_id, event.body.mqtt_state_update.app_id) != 0 && strcmp(app.second->entity_id, event.body.mqtt_state_update.entity_id) == 0)
                {
                    app.second->updateStateFromHASS(event.body.mqtt_state_update);
                }
            }
            // motor_notifier->requestUpdate(active_app->getMotorConfig());
        }
        else
        {
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
        }

        // cJSON_Delete(event.body.mqtt_state_update.state);
        break;
    default:
        break;
    }
    unlock();
}

TFT_eSprite *HassApps::renderActive()
{
    if (active_app == nullptr || apps.size() <= 1) // 1 is menu wich doesnt get removed when sync = 0 apps
    {
        return renderWaitingForHass();
    }
    return Apps::renderActive();
}

TFT_eSprite *HassApps::renderWaitingForHass()
{
    uint16_t center_vertical = TFT_HEIGHT / 2;
    uint16_t center_horizontal = TFT_WIDTH / 2;

    spr_->setTextDatum(CC_DATUM);
    spr_->setTextSize(1);
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(default_text_color);

    int8_t screen_name_label_h = spr_->fontHeight(1);

    spr_->drawString("Waiting for", center_horizontal, center_vertical - screen_name_label_h, 1);
    spr_->drawString("Home Assistant sync.", center_horizontal, center_vertical, 1);

    return this->spr_;
}