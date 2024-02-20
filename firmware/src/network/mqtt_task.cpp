#if SK_MQTT
#include "mqtt_task.h"

static const char *MQTT_TAG = "MQTT";
MqttTask::MqttTask(const uint8_t task_core) : Task{"mqtt", 2048 * 2, 1, task_core}
{
    mutex_app_sync_ = xSemaphoreCreateMutex();

    entity_state_to_send_queue_ = xQueueCreate(50, sizeof(EntityStateUpdate));
    assert(entity_state_to_send_queue_ != NULL);

    connectivity_status_queue_ = xQueueCreate(1, sizeof(ConnectivityState));
    assert(connectivity_status_queue_ != NULL);

    mqtt_state_ = {
        false,
        "",
        "",
    };
}

MqttTask::~MqttTask()
{
    vQueueDelete(entity_state_to_send_queue_);
    vQueueDelete(connectivity_status_queue_);
    vSemaphoreDelete(mutex_app_sync_);
}

void MqttTask::handleEvent(WiFiEvent event)
{

    if (event.type == MQTT_CREDENTIALS_RECIEVED)
    {

        ESP_LOGD("mqtt", "%s %d %s %s",
                 event.body.mqtt_connecting.host,
                 event.body.mqtt_connecting.port,
                 event.body.mqtt_connecting.user,
                 event.body.mqtt_connecting.password);

        setup(event.body.mqtt_connecting);
    }

    if (event.type == MQTT_SETUP)
    {
        connect();
    }

    if (event.type == SK_MQTT_CONNECTED)
    {
        init();
    }

    if (event.type == MQTT_CONNECTION_FAILED)
    {
        reconnect();
    }
}

void MqttTask::run()
{
    static uint32_t mqtt_pull;
    static uint32_t mqtt_push; // to prevent spam
    const uint16_t mqtt_push_interval_ms = 200;

    std::map<std::string, EntityStateUpdate> entity_states_to_send;
    EntityStateUpdate entity_state_to_process_;

    ConnectivityState connectivity_state_to_process_;

    static uint32_t last_mqtt_state_sent;

    while (1)
    {

        if (xQueueReceive(connectivity_status_queue_, &connectivity_state_to_process_, 0) == pdTRUE)
        {
            ESP_LOGD(MQTT_TAG, "Received connectivity state");
            if (last_connectivity_state_.ip_address != connectivity_state_to_process_.ip_address) // DOESNT CATCH ALL CHANGES!!
            {
                last_connectivity_state_ = connectivity_state_to_process_;
            }
            {
                last_connectivity_state_ = connectivity_state_to_process_;
            }
        }

        if (last_connectivity_state_.is_connected)
        {
            if (!mqttClient.connected())
            {
                WiFiEvent event;
                event.type = MQTT_CONNECTION_FAILED;
                publishEvent(event);
                vTaskDelay(pdMS_TO_TICKS(5));
                continue;
            }

            if (millis() - mqtt_pull > 1000)
            {
                mqttClient.loop();
                mqtt_pull = millis();
            }

            if (xQueueReceive(entity_state_to_send_queue_, &entity_state_to_process_, 0) == pdTRUE)
            {

                if (entity_state_to_process_.changed)
                {
                    entity_states_to_send[entity_state_to_process_.app_id] = entity_state_to_process_;
                }
            }

            if (millis() - mqtt_push > mqtt_push_interval_ms)
            {

                // iterate over all items in the map and push all not pushed yet
                for (auto i : entity_states_to_send)
                {
                    if (!entity_states_to_send[i.first].sent)
                    {
                        cJSON *json = cJSON_CreateObject();
                        cJSON_AddStringToObject(json, "app_id", entity_states_to_send[i.first].app_id);
                        cJSON_AddRawToObject(json, "state", entity_states_to_send[i.first].state);

                        String topic = "smartknob/" + WiFi.macAddress() + "/from_knob";

                        mqttClient.publish(topic.c_str(), cJSON_PrintUnformatted(json));

                        entity_states_to_send[i.first]
                            .sent = true;

                        cJSON_Delete(json);
                    }
                }

                mqtt_push = millis();
            }
        }

        if (millis() - last_mqtt_state_sent > 5000)
        {
            // MqttState state = {
            //     mqttClient.connected(),
            //     MQTT_SERVER,
            //     "smartknob",
            // };

            publishState(mqtt_state_);
            last_mqtt_state_sent = millis();
        }

        delay(5);
    }
}

bool MqttTask::setup(MQTTConfiguration config)
{
    WiFiEvent event;
    event.type = MQTT_SETUP;
    sprintf(event.body.mqtt_connecting.host, "%s", config.host);
    event.body.mqtt_connecting.port = config.port;
    sprintf(event.body.mqtt_connecting.user, "%s", config.user);
    sprintf(event.body.mqtt_connecting.password, "%s", config.password);

    config_ = config;

    mqttClient.setClient(wifi_client);
    mqttClient.setServer(config_.host, config_.port);
    mqttClient.setBufferSize(2048); // ADD BUFFER SIZE TO CONFIG? NO?
    mqttClient.setKeepAlive(60);
    mqttClient.setSocketTimeout(60);
    mqttClient.setCallback([this](char *topic, byte *payload, unsigned int length)
                           { this->callback(topic, payload, length); });

    publishEvent(event);
    return true;
}

bool MqttTask::connect()
{
    if (config_.host == "")
    {
        log("No host set");
        return false;
    }

    bool mqtt_connected = false;
    if (config_.user == "")
    {
        log("Connecting to MQTT without credentials");
        mqtt_connected = mqttClient.connect("smartknob");
    }
    else
    {
        log("Connecting to MQTT with credentials");
        mqtt_connected = mqttClient.connect("smartknob", config_.user, config_.password);
    }

    if (mqtt_connected)
    {
        WiFiEvent event;
        event.type = SK_MQTT_CONNECTED;
        publishEvent(event);
        log("MQTT client connected");
        return true;
    }
    else
    {
        WiFiEvent event;
        event.type = MQTT_CONNECTION_FAILED;
        publishEvent(event);
        log("MQTT connection failed");
    }
    return false;
}

bool MqttTask::reconnect()
{
    while (!mqttClient.connected())
    {
        log("Retrying MQTT connection..."); // TODO: add retry limit?
        if (connect())
        {
            log("MQTT reconnected");
            return true;
        }
        delay(5000);
    }
    return false;
}

bool MqttTask::init()
{
    char buf_[128];
    sprintf(buf_, "smartknob/%s/from_hass", WiFi.macAddress().c_str());
    mqttClient.subscribe(buf_);

    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "mac_address", WiFi.macAddress().c_str());
    mqttClient.publish("smartknob/init", cJSON_PrintUnformatted(json));
    cJSON_Delete(json);

    mqttClient.loop();

    WiFiEvent mqtt_connected_event;
    mqtt_connected_event.type = MQTT_INIT;
    publishEvent(mqtt_connected_event);
    return true;
}

void MqttTask::callback(char *topic, byte *payload, unsigned int length)
{
    cJSON *json_root = cJSON_Parse((char *)payload);
    cJSON *type = cJSON_GetObjectItem(json_root, "type");

    if (strcmp(type->valuestring, "sync") == 0)
    {
        log("sync");

        lock();
        apps = cJSON_GetObjectItem(json_root, "apps");
        unlock();

        publishAppSync(apps);
    }
}

void MqttTask::addStateListener(QueueHandle_t queue)
{
    state_listeners_.push_back(queue);
}

void MqttTask::publishState(const MqttState &state)
{
    for (auto listener : state_listeners_)
    {
        xQueueOverwrite(listener, &state);
    }
}

cJSON *MqttTask::getApps()
{
    lock();
    return apps;
}

void MqttTask::enqueueEntityStateToSend(EntityStateUpdate state)
{
    xQueueSendToBack(entity_state_to_send_queue_, &state, 0);
}

void MqttTask::setLogger(Logger *logger)
{
    logger_ = logger;
}

void MqttTask::log(const char *msg)
{
    if (logger_ != nullptr)
    {
        logger_->log(msg);
    }
}

void MqttTask::addAppSyncListener(QueueHandle_t queue)
{
    app_sync_listeners_.push_back(queue);
}

void MqttTask::publishAppSync(const cJSON *state)
{
    for (auto listener : app_sync_listeners_)
    {
        xQueueSend(listener, state, portMAX_DELAY);
    }
}

QueueHandle_t MqttTask::getConnectivityStateQueue()
{
    return connectivity_status_queue_;
}

void MqttTask::lock()
{
    xSemaphoreTake(mutex_app_sync_, portMAX_DELAY);
}
void MqttTask::unlock()
{
    xSemaphoreGive(mutex_app_sync_);
}

void MqttTask::setSharedEventsQueue(QueueHandle_t shared_events_queue)
{
    this->shared_events_queue = shared_events_queue;
}

void MqttTask::publishEvent(WiFiEvent event)
{
    xQueueSendToBack(shared_events_queue, &event, 0);
}

#endif
