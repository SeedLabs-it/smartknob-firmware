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

WifiTask::WifiTask(const uint8_t task_core) : Task{"wifi", 1024 * 6, 1, task_core}
{
    mutex_ = xSemaphoreCreateMutex();
    assert(mutex_ != NULL);

    wifi_events_queue = xQueueCreate(5, sizeof(WiFiEvent));
    assert(wifi_events_queue != NULL);

    // TODO make this more robust
    wifi_notifier = WiFiNotifier();
    wifi_notifier.setCallback([this](WiFiCommand command)
                              { this->handleCommand(command); });
}

WifiTask::~WifiTask()
{
    vSemaphoreDelete(mutex_);
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
    case RequestNewSTA:
        if (this->startNewWiFiSTA(command.body.wifi_sta_config))
        {
            this->startWebServer();
        }
        break;
    default:
        break;
    }
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

    char ssid[12] = "SKDK_A2R45C";
    char passphrase[9] = "12345678";

    // TODO, randomise hostname and password
    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.onEvent(OnWiFiEventGlobal);
    WiFi.softAP(ssid, passphrase);

    WiFiEvent event;
    event.type = SK_WIFI_AP_STARTED;

    strcpy(event.body.wifi_ap_started.ssid, ssid);
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
    WiFi.setAutoReconnect(true);
    WiFi.begin(wifi_config.ssid, wifi_config.passphrase);

    uint8_t max_tries = 6;
    uint8_t retry_count = 0;

    while (WiFi.status() != WL_CONNECTED)
    {
        if (WiFi.begin(wifi_config.ssid, wifi_config.passphrase) == WL_CONNECTED)
        {
            break;
        }
        else if (retry_count >= max_tries)
        {
            WiFiEvent wifi_event;
            wifi_event.type = SK_WIFI_STA_RETRY_LIMIT_REACHED;
            publishWiFiEvent(wifi_event);
            return false;
        }

        delay(5000);
        if (WiFi.status() != WL_CONNECTED)
        {
            WiFiEvent wifi_event;
            wifi_event.type = SK_WIFI_STA_CONNECTION_FAILED;
            wifi_event.body.error.body.wifi_error.retry_count = retry_count + 1;
            publishWiFiEvent(wifi_event);
        }
        retry_count++;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        WiFiEvent wifi_sta_connection_failed_event;
        wifi_sta_connection_failed_event.type = SK_WIFI_STA_CONNECTION_FAILED;
        publishWiFiEvent(wifi_sta_connecting_event);
        return false;
    }

    WiFiEvent wifi_sta_connected;
    wifi_sta_connected.type = SK_WIFI_STA_CONNECTED;
    strcpy(wifi_sta_connected.body.wifi_sta_connected.ssid, wifi_config.ssid);
    strcpy(wifi_sta_connected.body.wifi_sta_connected.passphrase, wifi_config.passphrase);

    publishWiFiEvent(wifi_sta_connected);
    return true;
}

bool WifiTask::startNewWiFiSTA(WiFiConfiguration wifi_config)
{
    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.setAutoReconnect(false);

    uint8_t max_tries = 6;
    uint8_t retry_count = 0;

    WiFiEvent wifi_sta_connecting;
    strcpy(wifi_sta_connecting.body.wifi_sta_connected.ssid, wifi_config.ssid);
    strcpy(wifi_sta_connecting.body.wifi_sta_connected.passphrase, wifi_config.passphrase);
    wifi_sta_connecting.type = SK_WIFI_STA_TRY_NEW_CREDENTIALS;
    publishWiFiEvent(wifi_sta_connecting);

    while (WiFi.status() != WL_CONNECTED)
    {
        if (WiFi.begin(wifi_config.ssid, wifi_config.passphrase) == WL_CONNECTED)
        {
            WiFiEvent wifi_sta_connecting;
            strcpy(wifi_sta_connecting.body.wifi_sta_connected.ssid, wifi_config.ssid);
            strcpy(wifi_sta_connecting.body.wifi_sta_connected.passphrase, wifi_config.passphrase);
            wifi_sta_connecting.type = SK_WIFI_STA_CONNECTED;
            publishWiFiEvent(wifi_sta_connecting);
            return true;
        }
        else if (retry_count >= max_tries)
        {
            WiFiEvent wifi_event;
            wifi_event.type = SK_WIFI_STA_TRY_NEW_CREDENTIALS_FAILED;
            publishWiFiEvent(wifi_event);
            return false;
        }

        delay(5000);
        retry_count++;
    }

    return true;
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
    server_->send(200, "text/html", "<form action='/submit' method='get'>"
                                    "SSID: <input type='text' name='ssid'><br>"
                                    "Password: <input type='text' name='password'><br>"
                                    "<input type='hidden' name='setup_type' value='wifi'>"
                                    "<input type='submit' value='Submit'>"
                                    "</form>");
}

void WifiTask::webHandlerMQTTForm()
{
    WiFiEvent event;
    event.type = SK_WEB_CLIENT_MQTT;
    publishWiFiEvent(event);

    server_->send(200, "text/html", "<form action='/submit' method='get'>"
                                    "MQTT SERVER: <input type='text' name='mqtt_server'><br>"
                                    "MQTT PORT: <input type='number' name='mqtt_port'><br>"
                                    "MQTT USER: <input type='text' name='mqtt_user'><br>"
                                    "MQTT PASSWORD: <input type='text' name='mqtt_password'><br>"
                                    "<input type='hidden' name='setup_type' value='mqtt'>"
                                    "<input type='submit' value='Submit'>"
                                    "</form>");
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

    if (startWiFiSTA(wifi_config))
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

    publishWiFiEvent(event);

    // preferences.begin("mqtt", false);
    // preferences.clear();

    // preferences.putString("mqtt_server", mqtt_server);
    // preferences.putUInt("mqtt_port", mqtt_port);
    // preferences.putString("mqtt_user", mqtt_user);
    // preferences.putString("mqtt_password", mqtt_password);

    // preferences.end();

    // WiFi.mode(WIFI_STA);
    // WiFi.softAPdisconnect(true);
    server_->send(200, "text/html", "MQTT setup complete!");
}

void WifiTask::startWebServer()
{
    server_ = new WebServer(80);
    ElegantOTA.begin(server_);

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

    static uint32_t last_wifi_status;

    while (1)
    {

        if (is_webserver_started)
        {
            server_->handleClient();
            ElegantOTA.loop();
        }

        if (millis() - last_wifi_status > 5000)
        {
            updateWifiState();
            last_wifi_status = millis();

            // if (!WiFi.isConnected())
            // {
            //     ESP_LOGD("wifi", "WiFi not connected");
            //     WiFiEvent wifi_sta_connection_failed_event;
            //     wifi_sta_connection_failed_event.type = SK_WIFI_STA_CONNECTION_FAILED;
            //     publishWiFiEvent(wifi_sta_connection_failed_event);
            // }
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
        WiFi.SSID().c_str(),
        WiFi.localIP().toString().c_str(),
        WiFi.getMode() == WIFI_AP ? true : false,
        WiFi.softAPIP(),
        WiFi.softAPgetStationNum() > 0 ? true : false,
    };

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
