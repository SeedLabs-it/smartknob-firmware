#if SK_WIFI
#include "wifi_task.h"
#include "../semaphore_guard.h"
#include "../util.h"
#include "cJSON.h"
#include "../root_task.h"

#if SK_NETWORKING
#include "wifi_config.h"
#endif

static const char *WIFI_TAG = "WIFI";

QueueHandle_t wifi_events_queue;

// example article
// https://techtutorialsx.com/2021/01/04/esp32-soft-ap-and-station-modes/

WifiTask::WifiTask(const uint8_t task_core, Configuration &configuration) : Task{"wifi", 1024 * 13, 1, task_core}, configuration_(configuration)
{
    mutex_ = xSemaphoreCreateMutex();
    assert(mutex_ != NULL);

    wifi_events_queue = xQueueCreate(3, sizeof(WiFiEvent));
    assert(wifi_events_queue != NULL);

    // TODO make this more robust
    wifi_notifier = WiFiNotifier();
    wifi_notifier.setCallback([this](WiFiCommand command)
                              { this->handleCommand(command); });

    WiFi.setAutoReconnect(true);
}

WifiTask::~WifiTask()
{
    vSemaphoreDelete(mutex_);
}

void WifiTask::setConfig(WiFiConfiguration config)
{
    wifi_config_ = config;
}

void WifiTask::handleCommand(WiFiCommand command)
{
    switch (command.type)
    {
    case RequestAP:
        this->startWiFiAP();
        this->startWebServer();
        break;
    case RequestSTA:
        if (this->startWiFiSTA(command.body.wifi_sta_config))
        {
            this->startWebServer();
        }
        break;
    default:
        break;
    }
}

void WifiTask::resetRetryCount()
{
    retry_count = 0;
}

void OnWiFiEventGlobal(WiFiEvent_t event)
{

    WiFiEvent wifi_event;

    switch (event)
    {
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
        wifi_event.type = SK_AP_CLIENT;
        wifi_event.body.ap_client.connected = true;

        xQueueSendToBack(wifi_events_queue, &wifi_event, 0);
        break;
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:

        wifi_event.type = SK_AP_CLIENT;
        wifi_event.body.ap_client.connected = false;

        xQueueSendToBack(wifi_events_queue, &wifi_event, 0);
        break;
    default:
        break;
    }
}

void WifiTask::startWiFiAP()
{
    char passphrase[9] = "12345678";

    // TODO, randomise hostname and password
    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.onEvent(OnWiFiEventGlobal);
    WiFi.softAP(wifi_config_.knob_id, passphrase);

    WiFiEvent event;
    event.type = SK_WIFI_AP_STARTED;

    strcpy(event.body.wifi_ap_started.ssid, wifi_config_.knob_id);
    strcpy(event.body.wifi_ap_started.passphrase, passphrase);

    // DEBUG: added delay for testing, remove this on release
    vTaskDelay(pdMS_TO_TICKS(2000));

    publishWiFiEvent(event);
}

bool WifiTask::startWiFiSTA(WiFiConfiguration wifi_config)
{

    WiFiEvent wifi_sta_connecting_event;

    wifi_sta_connecting_event.type = SK_WIFI_STA_CONNECTING;
    sprintf(wifi_sta_connecting_event.body.wifi_sta_connecting.ssid, "%s", wifi_config.ssid);
    sprintf(wifi_sta_connecting_event.body.wifi_sta_connecting.passphrase, "%s", wifi_config.passphrase);
    publishWiFiEvent(wifi_sta_connecting_event);

    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.begin(wifi_config.ssid, wifi_config.passphrase);

    WiFiEvent wifi_sta_connected;
    wifi_sta_connected.type = SK_WIFI_STA_CONNECTED;
    strcpy(wifi_sta_connected.body.wifi_sta_connected.ssid, wifi_config.ssid);
    strcpy(wifi_sta_connected.body.wifi_sta_connected.passphrase, wifi_config.passphrase);

    setConfig(wifi_config);
    is_config_set = true;

    publishWiFiEvent(wifi_sta_connected);
    return true;
}

bool WifiTask::tryNewCredentialsWiFiSTA(WiFiConfiguration wifi_config)
{
    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.setAutoReconnect(true);

    WiFiEvent wifi_sta_connecting;
    strcpy(wifi_sta_connecting.body.wifi_sta_connected.ssid, wifi_config.ssid);
    strcpy(wifi_sta_connecting.body.wifi_sta_connected.passphrase, wifi_config.passphrase);
    wifi_sta_connecting.type = SK_WIFI_STA_TRY_NEW_CREDENTIALS;
    publishWiFiEvent(wifi_sta_connecting);

    WiFi.begin(wifi_config.ssid, wifi_config.passphrase);

    uint32_t timeout_at = millis() + 30000;

    while (WiFi.status() != WL_CONNECTED && timeout_at > millis())
    {
        delay(250);
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        WiFiEvent wifi_sta_connected;
        strcpy(wifi_sta_connected.body.wifi_sta_connected.ssid, wifi_config.ssid);
        strcpy(wifi_sta_connected.body.wifi_sta_connected.passphrase, wifi_config.passphrase);
        wifi_sta_connected.type = SK_WIFI_STA_CONNECTED;

        setConfig(wifi_config);
        is_config_set = true;

        publishWiFiEvent(wifi_sta_connected);

        return true;
    }

    WiFi.disconnect();

    WiFiEvent wifi_event;
    wifi_event.type = SK_WIFI_STA_TRY_NEW_CREDENTIALS_FAILED;
    publishWiFiEvent(wifi_event);

    return false;
}

void WifiTask::webHandlerWiFiCredentials()
{
    // TODO: redo wifi reconnection

    WiFi.disconnect();

    cJSON *root = cJSON_Parse(server_->arg("plain").c_str());
    std::string ssid = std::string(cJSON_GetStringValue(cJSON_GetObjectItem(root, "ssid")));
    std::string passphrase = std::string(cJSON_GetStringValue(cJSON_GetObjectItem(root, "password")));
    cJSON_Delete(root);

    if (ssid.length() == 0 || passphrase.length() == 0)
    {
        // TODO handle
        //  server_->sendHeader("Location", "/");
        //  server_->send(302, "text/plain", "Invalid WiFi credentials!");
        return;
    }

    // TODO send event connecting to wifi

    WiFiConfiguration wifi_config;
    sprintf(wifi_config.ssid, "%s", ssid.c_str());
    sprintf(wifi_config.passphrase, "%s", passphrase.c_str());
    sprintf(wifi_config.knob_id, "%s", wifi_config_.knob_id);

    if (tryNewCredentialsWiFiSTA(wifi_config))
    {
        WiFiEvent wifi_sta_connected;
        wifi_sta_connected.type = SK_WIFI_STA_CONNECTED_NEW_CREDENTIALS;
        sprintf(wifi_sta_connected.body.wifi_sta_connected.ssid, "%s", ssid.c_str());
        sprintf(wifi_sta_connected.body.wifi_sta_connected.passphrase, "%s", passphrase.c_str());

        publishWiFiEvent(wifi_sta_connected);

        server_->send(200, "text/plain", "Connected to WiFi!");
    }
    else
    {
        server_->send(302, "text/plain", "Invalid WiFi credentials!");
    }
}

void WifiTask::webHandlerMQTTCredentials()
{
    delay(1000);
    if (WiFi.status() != WL_CONNECTED)
    {
        // TODO handle
        //  server_->sendHeader("Location", "/mqtt");
        //  server_->send(302, "text/plain", "Connect to WiFi first!");
        return;
    }
    retry_mqtt = false;

    cJSON *root = cJSON_Parse(server_->arg("plain").c_str());
    const char *server = cJSON_GetStringValue(cJSON_GetObjectItem(root, "server"));
    uint16_t port = (uint16_t)cJSON_GetNumberValue(cJSON_GetObjectItem(root, "portInt"));
    const char *username = cJSON_GetStringValue(cJSON_GetObjectItem(root, "username"));
    const char *password = cJSON_GetStringValue(cJSON_GetObjectItem(root, "password"));
    cJSON_Delete(root);

    WiFiEvent event;
    event.type = SK_MQTT_NEW_CREDENTIALS_RECIEVED;
    sprintf(event.body.mqtt_connecting.host, "%s", server);
    event.body.mqtt_connecting.port = port;
    sprintf(event.body.mqtt_connecting.user, "%s", username);
    sprintf(event.body.mqtt_connecting.password, "%s", password);
    LOGD("MQTT credentials recieved: %s %d %s %s %s", event.body.mqtt_connecting.host, event.body.mqtt_connecting.port, event.body.mqtt_connecting.user, event.body.mqtt_connecting.password, wifi_config_.knob_id);
    sprintf(event.body.mqtt_connecting.knob_id, "%s", wifi_config_.knob_id);

    publishWiFiEvent(event);

    while (!retry_mqtt)
    {
        delay(100);
    }
    if (mqtt_connected)
    {
        server_->send(200, "text/plain", "Connected to MQTT!");
    }
    else
    {
        server_->send(302, "text/plain", "Connecting to MQTT with provided credentials failed!");
    }
}

void WifiTask::webHandlerSpotifyCredentials()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        // TODO handle
        return;
    }

    WiFiEvent event;

    event.type = SK_SPOTIFY_ACCESS_TOKEN_RECEIVED;
    publishWiFiEvent(event);

    PB_SpotifyConfig *spotify_config_ = (PB_SpotifyConfig *)heap_caps_malloc(sizeof(PB_SpotifyConfig), MALLOC_CAP_SPIRAM);

    cJSON *root = cJSON_Parse(server_->arg("plain").c_str());
    cJSON *spotify_response = cJSON_GetObjectItem(root, "spotify_response");
    strcpy(spotify_config_->base64_id_and_secret, cJSON_GetStringValue(cJSON_GetObjectItem(root, "base64_id_and_secret")));
    strcpy(spotify_config_->access_token, cJSON_GetStringValue(cJSON_GetObjectItem(spotify_response, "access_token")));
    strcpy(spotify_config_->token_type, cJSON_GetStringValue(cJSON_GetObjectItem(spotify_response, "token_type")));
    strcpy(spotify_config_->scope, cJSON_GetStringValue(cJSON_GetObjectItem(spotify_response, "scope")));
    spotify_config_->expires_in = cJSON_GetNumberValue(cJSON_GetObjectItem(spotify_response, "expires_in"));
    strcpy(spotify_config_->refresh_token, cJSON_GetStringValue(cJSON_GetObjectItem(spotify_response, "refresh_token")));
    spotify_config_->timestamp = cJSON_GetNumberValue(cJSON_GetObjectItem(spotify_response, "timestamp"));
    cJSON_Delete(root);

    // SpotifyApi spotify(*spotify_config_);

    // if (spotify.isAvailable())
    if (true) // TODO actually validate
    {
        configuration_.setSpotifyConfig(*spotify_config_);
        event.type = SK_SPOTIFY_ACCESS_TOKEN_VALIDATED; // TODO actually validate

        publishWiFiEvent(event);
        server_->send(200, "text/plain", "Connected to WiFi!");
        return;
    }
    server_->send(302, "text/plain", "Invalid Spotify credentials!");
}

void WifiTask::mqttConnected(bool connected)
{
    mqtt_connected = connected;
}

void WifiTask::retryMqtt(bool retry)
{
    retry_mqtt = retry;
}

void WifiTask::downloadConfig()
{
    // LOGV(PB_LogLevel_INFO, "Download config");
    LOGI("Download config");
    if (FFat.exists(CONFIG_PATH))
    {
        File file = FFat.open(CONFIG_PATH, "r");
        server_->sendHeader("Content-Disposition", "attachment; filename=" + String(file.name()));

        server_->streamFile(file, "application/octet-stream");
        file.close();
    }
    else
    {
        server_->send(404, "text/plain", "File Not Found");
    }
}

void WifiTask::startWebServer()
{
    server_ = new WebServer(80);
    ElegantOTA.begin(server_);

#if SK_ELEGANTOTA_PRO
    ElegantOTA.setID(wifi_config_.knob_id);
    ElegantOTA.setFWVersion(RELEASE_VERSION ? RELEASE_VERSION : "DEV");
    ElegantOTA.setFirmwareMode(true);
    ElegantOTA.setFilesystemMode(false);
    ElegantOTA.setTitle("SKDK - Update");
#endif

    // TODO: do local files rendering
    // TODO: make this page work async with animations on UI

    server_->on("/download/config", HTTP_GET, [this]()
                { this->downloadConfig(); });

    server_->on("/wifi", HTTP_POST, [this]()
                { this->webHandlerWiFiCredentials(); });

    server_->on("/mqtt", HTTP_POST, [this]()
                { this->webHandlerMQTTCredentials(); });

    server_->on("/spotify", HTTP_POST, [this]()
                { this->webHandlerSpotifyCredentials(); });

    if (!FFat.begin(true)) // TODO check if viable to leave ffat open for duration of active webapp???
    {
        server_->send(404, "text/plain", "FileSystem not mounted");
    }
    server_->serveStatic("/", FFat, "/setup/");

    server_->begin();

    LOGV(LOG_LEVEL_INFO, "WebServer started");

    is_webserver_started = true;
    // TODO: send event to
}

void WifiTask::run()
{
    // TODO: set hostname
    static uint32_t last_wifi_status;
    static uint32_t last_wifi_status_new;

    static unsigned long last_handle_client_ms_ = 0;
    bool has_been_connected = false;

    unsigned long last_run_ms_ = 0;

    while (1)
    {
        last_run_ms_ = millis();
        // DONT RUN TO OFTEN, TODO VERIFY IT WORKS WITH UPLOAD FIRMWARE
        if (is_webserver_started) // WEBSERVER IS ALWAYS STARTED AFTER ONBOARDING AND BOOT SO WIFI CONNECTED LOOP CAN LIVE HERE FOR NOW
        {
            server_->handleClient();
            ElegantOTA.loop();
        }

        if (is_config_set && last_run_ms_ - last_wifi_status > 5000)
        {
            LOGE("FREE HEAP: %d", ESP.getFreeHeap());
            LOGE("FREE MIN HEAP: %d", ESP.getMinFreeHeap());
            // LOGE("FREE PSRAM: %d", ESP.getFreePsram());

            // Serial.printf("FREE HEAP %d \n", ESP.getFreeHeap());
            // Serial.printf("FREE MIN HEAP %d \n", ESP.getMinFreeHeap());

            updateWifiState();
            last_wifi_status = last_run_ms_;
        }

        if (is_config_set && last_run_ms_ - last_wifi_status_new > 3000 && WiFi.status() != WL_CONNECTED && retry_count < 3)
        {
            LOGV(LOG_LEVEL_DEBUG, "WiFi status: %d", WiFi.status());
            LOGV(LOG_LEVEL_DEBUG, "WiFi connected: %d", WiFi.isConnected());
            LOGV(LOG_LEVEL_DEBUG, "Retry count: %d", retry_count);
            LOGV(LOG_LEVEL_DEBUG, "Last_wifi_status_new: %d", last_wifi_status_new);

            WiFi.begin(wifi_config_.ssid, wifi_config_.passphrase);
            while (WiFi.status() != WL_CONNECTED)
            {
                if (!has_been_connected || retry_count > 0)
                {
                    WiFiEvent event;
                    WiFiEventBody wifi_event_body;
                    wifi_event_body.error.type = WIFI_ERROR;
                    wifi_event_body.error.body.wifi_error.retry_count = retry_count + 1;

                    event.type = SK_WIFI_STA_CONNECTION_FAILED;
                    event.body = wifi_event_body;
                    event.sent_at = millis();
                    publishWiFiEvent(event);
                }

                if (retry_count > 2)
                {
                    WiFi.disconnect();
                    LOGW("Retry limit reached...");
                    WiFiEvent event;
                    event.type = SK_WIFI_STA_RETRY_LIMIT_REACHED;
                    publishWiFiEvent(event);
                    break;
                }
                delay(10000);
                retry_count++;
            }
            if (WiFi.status() == WL_CONNECTED)
            {
                has_been_connected = true;
                retry_count = 0;
                WiFiEvent reset_error;
                reset_error.type = SK_RESET_ERROR;
                publishWiFiEvent(reset_error);
            }
            last_wifi_status_new = millis();
        }

        wifi_notifier.loopTick();

        delay(1);
    }
}

WiFiNotifier *WifiTask::getNotifier()
{
    return &wifi_notifier;
}

void WifiTask::updateWifiState()
{
    int8_t rssi = WiFi.RSSI();
    uint8_t signal_strenth_status;

    if (rssi < -110)
    {
        signal_strenth_status = 4;
    }
    else if (rssi < -100)
    {
        signal_strenth_status = 3;
    }
    else if (rssi < -85)
    {
        signal_strenth_status = 2;
    }
    else if (rssi < -70)
    {
        signal_strenth_status = 1;
    }
    else
    {
        signal_strenth_status = 0;
    }
    // harvest state;
    ConnectivityState state = {
        WiFi.isConnected(),
        WiFi.RSSI(),
        signal_strenth_status,
        "",
        "",
        WiFi.getMode() == WIFI_AP ? true : false,
        WiFi.softAPIP(),
        WiFi.softAPgetStationNum() > 0 ? true : false,
    };

    sprintf(state.ssid, "%s", WiFi.SSID().c_str());
    // TODO: look into how to store and retrieve ip in a better way.
    sprintf(state.ip_address, "%s", WiFi.localIP().toString().c_str());

    publishState(state);
}

void WifiTask::addStateListener(QueueHandle_t queue)
{
    state_listeners_.push_back(queue);
}

void WifiTask::publishState(const ConnectivityState &state)
{
    for (auto listener : state_listeners_)
    {
        xQueueOverwrite(listener, &state);
    }
}

QueueHandle_t WifiTask::getWiFiEventsQueue()
{
    return wifi_events_queue;
}

void WifiTask::publishWiFiEvent(WiFiEvent event)
{
    event.sent_at = millis();
    xQueueSendToBack(wifi_events_queue, &event, 0);
}

#endif
