#include "hass_apps.h"

HassApps::HassApps() : Apps()
{
}

void HassApps::sync(cJSON *apps_)
{
    clear();
    uint16_t app_position = 0;

    cJSON *json_app = NULL;
    cJSON_ArrayForEach(json_app, apps_)
    {

        cJSON *json_app_slug = cJSON_GetObjectItemCaseSensitive(json_app, "app_slug");
        cJSON *json_app_id = cJSON_GetObjectItemCaseSensitive(json_app, "app_id");
        cJSON *json_friendly_name = cJSON_GetObjectItemCaseSensitive(json_app, "friendly_name");
        loadApp(app_position, json_app_slug->valuestring, cJSON_Print(json_app_id), cJSON_Print(json_friendly_name));

        app_position++;
    }

    SettingsApp *settings_app = new SettingsApp(this->spr_);
    add(app_position, settings_app);

    updateMenu();
    // cJSON_Delete(apps_);
}