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

WifiTask::WifiTask(const uint8_t task_core) : Task{"wifi", 1024 * 7, 1, task_core}
{
    mutex_ = xSemaphoreCreateMutex();
    assert(mutex_ != NULL);

    wifi_events_queue = xQueueCreate(5, sizeof(WiFiEvent));
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
    config_ = config;
    // is_config_set = true;
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

    case SYSTEM_EVENT_STA_CONNECTED:
        ESP_LOGD("wifi", "ESP32 Connected to WiFi Network");
        break;
    // case SYSTEM_EVENT_AP_START:
    //     Serial.println("ESP32 soft AP started");
    //     break;
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
    WiFi.softAP(config_.knob_id, passphrase);

    WiFiEvent event;
    event.type = SK_WIFI_AP_STARTED;

    strcpy(event.body.wifi_ap_started.ssid, config_.knob_id);
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

    config_ = wifi_config;
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

        config_ = wifi_config;
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

void WifiTask::webHandlerWiFiForm()
{
    WiFiEvent event;
    event.type = SK_WEB_CLIENT;
    event.body.web_client.connected = true;

    publishWiFiEvent(event);
    // TODO trigger event that user is connected

    // if (WiFi.isConnected()) {
    //     server_->sendHeader("Location", "/mqtt");
    //     server_->send(302, "text/plain", "Connected to WiFi redirecting to MQTT setup!");
    // } else {
    server_->send(200, "text/html", R"(<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1.0"><style>body {background-color: #1f1f1f;color: #fff;font-family: Arial, sans-serif;padding: 20px;}form {background-color: #333;padding: 20px;border-radius: 10px;max-width: 400px;margin: 0 auto;display: flex;flex-direction: column;}h2,label {margin-right: 6px;margin-left: 6px;}div {display: flex;align-items: center;justify-content: space-between;padding: 6px 0;}input {width: calc(100% - 12px);margin-bottom: 10px;padding: 10px;box-sizing: border-box;margin-right: 6px;margin-left: 6px;margin-bottom: 12px;}input[type='checkbox'] {width: 24px;padding: 0;margin: 0;margin-right: 6px;}input[type='submit'] {background-color: #4CAF50;color: #fff;border: none;border-radius: 4px;cursor: pointer;font-weight: bold;margin-top: 6px;}input[type='submit']:hover {background-color: #45a049;}</style></head><body><form action='/submit' method='get'><h2>WIFI</h2><label for='ssid'>SSID</label><input type='text' id='ssid' name='ssid'><label for='password'>Password</label><input type='password' id='password' name='password'><input type='hidden' name='setup_type' value='wifi'><input type='submit' value='Submit'></form></body></html>)");
}

void WifiTask::webHandlerMQTTForm()
{
    WiFiEvent event;
    event.type = SK_WEB_CLIENT_MQTT;
    publishWiFiEvent(event);

    server_->send(200, "text/html", R"(<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1.0"><style>body {background-color: #1f1f1f;color: #fff;font-family: Arial, sans-serif;padding: 20px;}form {background-color: #333;padding: 20px;border-radius: 10px;max-width: 400px;margin: 0 auto;display: flex;flex-direction: column;}h2,label {margin-right: 6px;margin-left: 6px;}div {display: flex;align-items: center;justify-content: space-between;padding: 6px 0;}input {width: calc(100% - 12px);margin-bottom: 10px;padding: 10px;box-sizing: border-box;margin-right: 6px;margin-left: 6px;margin-bottom: 12px;}input[type='checkbox'] {width: 24px;padding: 0;margin: 0;margin-right: 6px;}input[type='submit'] {background-color: #4CAF50;color: #fff;border: none;border-radius: 4px;cursor: pointer;font-weight: bold;margin-top: 6px;}input[type='submit']:hover {background-color: #45a049;}</style></head><body><form action='/submit' method='get'><h2>MQTT</h2><label for='mqtt_server'>SERVER</label><input type='text' id='mqtt_server' name='mqtt_server'><label for='mqtt_port'>PORT</label><input type='number' id='mqtt_port' name='mqtt_port'><div><label for='toggle_mqtt' style='font-weight: normal;'>Toggle Username/Password</label><input type='checkbox' id='toggle_mqtt' onclick='toggleMqttFields();'></div><div id='mqtt_fields' style='display: none;'><label for='mqtt_user'>USER</label><input type='text' id='mqtt_user' name='mqtt_user'><label for='mqtt_password'>PASSWORD</label><input type='password' id='mqtt_password' name='mqtt_password'></div><input type='hidden' name='setup_type' value='mqtt'><input type='submit' value='Submit'></form><script>function toggleMqttFields() {var mqttFields = document.getElementById('mqtt_fields');if (mqttFields.style.display === 'none') {mqttFields.style.display = 'block';} else {mqttFields.style.display = 'none';}}</script></body></html>)");
}

void WifiTask::webHandlerWiFiCredentials()
{
    // TODO: redo wifi reconnection

    WiFi.disconnect();

    String ssid = server_->arg("ssid");
    String passphrase = server_->arg("password");

    // TODO send event connecting to wifi

    WiFiConfiguration wifi_config;
    sprintf(wifi_config.ssid, "%s", ssid.c_str());
    sprintf(wifi_config.passphrase, "%s", passphrase.c_str());
    sprintf(wifi_config.knob_id, "%s", config_.knob_id);

    if (tryNewCredentialsWiFiSTA(wifi_config))
    {
        WiFiEvent wifi_sta_connected;
        wifi_sta_connected.type = SK_WIFI_STA_CONNECTED_NEW_CREDENTIALS;
        sprintf(wifi_sta_connected.body.wifi_sta_connected.ssid, "%s", ssid.c_str());
        sprintf(wifi_sta_connected.body.wifi_sta_connected.passphrase, "%s", passphrase.c_str());

        publishWiFiEvent(wifi_sta_connected);

        server_->sendHeader("Location", "/mqtt");
        server_->send(302, "text/plain", "Connected to WiFi redirecting to MQTT setup!");
    }
    else
    {
        server_->sendHeader("Location", "/");
        server_->send(302, "text/plain", "Invalid WiFi credentials!");
    }
}

void WifiTask::webHandlerMQTTCredentials()
{
    retry_mqtt = false;
    String mqtt_server = server_->arg("mqtt_server");
    uint32_t mqtt_port = server_->arg("mqtt_port").toInt();
    String mqtt_user = server_->arg("mqtt_user");
    String mqtt_password = server_->arg("mqtt_password");

    WiFiEvent event;
    event.type = SK_MQTT_NEW_CREDENTIALS_RECIEVED;
    sprintf(event.body.mqtt_connecting.host, "%s", mqtt_server.c_str());
    event.body.mqtt_connecting.port = mqtt_port;
    sprintf(event.body.mqtt_connecting.user, "%s", mqtt_user.c_str());
    sprintf(event.body.mqtt_connecting.password, "%s", mqtt_password.c_str());
    ESP_LOGD(WIFI_TAG, "MQTT credentials recieved: %s %d %s %s %s", event.body.mqtt_connecting.host, event.body.mqtt_connecting.port, event.body.mqtt_connecting.user, event.body.mqtt_connecting.password, config_.knob_id);
    sprintf(event.body.mqtt_connecting.knob_id, "%s", config_.knob_id);

    publishWiFiEvent(event);

    // server_->send(200, "text/html", "MQTT credentials recieved!");

    while (!retry_mqtt)
    {
        delay(100);
    }
    if (mqtt_connected)
    {
        server_->send(200, "text/html", R"(<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1.0"><style>body {background-color: #1f1f1f;color: #fff;font-family: Arial, sans-serif;padding: 20px;}form {background-color: #333;padding: 20px;border-radius: 10px;max-width: 400px;margin: 0 auto;display: flex;flex-direction: column;}h2,label {margin-right: 6px;margin-left: 6px;}div {display: flex;align-items: center;justify-content: space-between;padding: 6px 0;}input {width: calc(100% - 12px);margin-bottom: 10px;padding: 10px;box-sizing: border-box;margin-right: 6px;margin-left: 6px;margin-bottom: 12px;}input[type='checkbox'] {width: 24px;padding: 0;margin: 0;margin-right: 6px;}input[type='submit'] {background-color: #4CAF50;color: #fff;border: none;border-radius: 4px;cursor: pointer;font-weight: bold;margin-top: 6px;}input[type='submit']:hover {background-color: #45a049;}</style></head><body><form><h2>Setup done, continue in Home Assistant!</h2></form></body></html>)");
        return;
    }
    server_->sendHeader("Location", "/mqtt");
    server_->send(302, "text/plain", "Connecting to MQTT with provided credentials failed!");
}

void WifiTask::mqttConnected(bool connected)
{
    mqtt_connected = connected;
}

void WifiTask::retryMqtt(bool retry)
{
    retry_mqtt = retry;
}

void WifiTask::startWebServer()
{
    server_ = new WebServer(80);
    ElegantOTA.begin(server_);

#if SK_ELEGANTOTA_PRO
    ElegantOTA.setID(config_.knob_id);
    ElegantOTA.setFWVersion(RELEASE_VERSION);
    ElegantOTA.setFirmwareMode(true);
    ElegantOTA.setFilesystemMode(false);
    ElegantOTA.setTitle("SKDK - Update");
#endif

    // TODO: do local files rendering
    // TODO: make this page work async with animations on UI

    server_->on("/", HTTP_GET, [this]()
                { this->webHandlerWiFiForm(); });

    server_->on("/mqtt", HTTP_GET, [this]()
                { this->webHandlerMQTTForm(); });

    server_->on("/submit", [this]()
                {
        if (server_->arg("setup_type") == "wifi")
        {
            this->webHandlerWiFiCredentials();
        }
        else if (server_->arg("setup_type") == "mqtt")
        {
            this->webHandlerMQTTCredentials();
        } });

    server_->begin();

    log("WebServer started");
    is_webserver_started = true;
    // TODO: send event to
}

void WifiTask::run()
{
    // TODO: set hostname
    static uint32_t last_wifi_status;
    static uint32_t last_wifi_status_new;
    bool has_been_connected = false;

    while (1)
    {
        if (is_webserver_started) // WEBSERVER IS ALWAYS STARTED AFTER ONBOARDING AND BOOT SO WIFI CONNECTED LOOP CAN LIVE HERE FOR NOW
        {
            server_->handleClient();
            ElegantOTA.loop();
        }

        if (is_config_set && millis() - last_wifi_status > 5000)
        {
            updateWifiState();
            last_wifi_status = millis();
        }

        if (is_config_set && millis() - last_wifi_status_new > 3000 && WiFi.status() != WL_CONNECTED && retry_count < 3)
        {
            ESP_LOGD(WIFI_TAG, "WiFi status: %d", WiFi.status());
            ESP_LOGD(WIFI_TAG, "WiFi connected: %d", WiFi.isConnected());
            ESP_LOGD(WIFI_TAG, "Retry count: %d", retry_count);
            ESP_LOGD(WIFI_TAG, "last_wifi_status_new: %d", last_wifi_status_new);

            WiFi.begin(config_.ssid, config_.passphrase);
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
                    log("Retry limit reached...");
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

        delay(5);
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

void WifiTask::setLogger(Logger *logger)
{
    logger_ = logger;
}

void WifiTask::log(const char *msg)
{
    if (logger_ != nullptr)
    {
        logger_->log(msg);
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
