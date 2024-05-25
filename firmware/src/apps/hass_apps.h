#pragma once
#include "apps.h"
#include "../util.h"
#include "../events/events.h"

class HassApps : public Apps
{
public:
    // HassApps(){};
    HassApps(SemaphoreHandle_t mutex) : Apps(mutex)
    {
        lv_obj_t *label = lv_label_create(waiting_for_hass);
        lv_label_set_text(label, "Waiting for Home Assistant");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    };
    void sync(cJSON *json_apps)
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

            if (json_app_slug == NULL || json_app_id == NULL || json_entity_id == NULL || json_friendly_name == NULL)
            {
                LOGE("Invalid data.");
                continue;
            }

            loadApp(app_position, json_app_slug->valuestring, json_app_id->valuestring, json_friendly_name->valuestring, json_entity_id->valuestring);

            app_position++;
        }

        // SettingsApp *settings_app = new SettingsApp(this->spr_);
        // add(app_position, settings_app);

        updateMenu();
        setMotorNotifier(motor_notifier);
        // cJSON_Delete(json_apps); //DELETING DELETES POINTERS NEEDED TO DISPLAY FRIENDLY NAME ON APPS HMMMM
    }
    void handleEvent(WiFiEvent event)
    {
        SemaphoreGuard lock(app_mutex_);
        // lock();
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
                    LOGW("App not found");
                }
            }

            // cJSON_Delete(event.body.mqtt_state_update.state);
            break;
        default:
            break;
        }
        // unlock();
    }
    void handleNavigationEvent(NavigationEvent event)
    {
        if (active_app == nullptr || apps.size() <= 1) // 1 is menu wich doesnt get removed when sync = 0 apps
        {
            return;
        }
        return Apps::handleNavigationEvent(event);
    }

    // TFT_eSprite *renderActive();
    void renderActive()
    {
        LOGE("renderActive");
        LOGE("active_app: %s", active_app->app_id);
        LOGE("apps.size(): %d", apps.size());
        if (active_app == nullptr || apps.size() <= 1) // 1 is menu wich doesnt get removed when sync = 0 apps
        {
            return lv_screen_load(waiting_for_hass);
        }
        return Apps::renderActive();
    }

private:
    lv_obj_t *waiting_for_hass = lv_obj_create(NULL);

    uint16_t default_text_color = rgbToUint32(150, 150, 150);
    uint16_t accent_text_color = rgbToUint32(128, 255, 80);

    // TFT_eSprite *renderWaitingForHass();
};