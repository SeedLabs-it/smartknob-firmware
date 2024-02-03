#if SK_NETWORKING
#include "mqtt_task.h"

#include "wifi_config.h"

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
            if (mqtt_state_.server == "")
            {
                setup_mqtt();
            }

            if (!mqttClient.connected())
            {
                reconnect_mqtt();
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

void MqttTask::setup_mqtt()
{
    const char *mqtt_host = MQTT_SERVER;
    const uint16_t mqtt_port = MQTT_PORT;
    const char *mqtt_user = MQTT_USER;
    const char *mqtt_pass = MQTT_PASSWORD;

    log("Starting MQTT client");

    mqttClient.setClient(wifi_client);
    mqttClient.setBufferSize(512);
    mqttClient.setKeepAlive(60);
    mqttClient.setSocketTimeout(60);
    mqttClient.setServer(mqtt_host, mqtt_port);
    mqttClient.setCallback([this](char *topic, byte *payload, unsigned int length)
                           { this->callback_mqtt(topic, payload, length); });

    if (!mqttClient.connected())
    {
        reconnect_mqtt();
    }
    mqttClient.loop();

    mqtt_state_.is_connected = mqttClient.connected();
    mqtt_state_.server = mqtt_host;
    mqtt_state_.client_id = "smartknob";

    char buf_[128];
    sprintf(buf_, "smartknob/%s/from_hass", WiFi.macAddress().c_str());
    mqttClient.subscribe(buf_);

    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "mac_address", WiFi.macAddress().c_str());
    mqttClient.publish("smartknob/init", cJSON_PrintUnformatted(json));

    // PREVENTS MEMORY LEAK???
    cJSON_Delete(json);
}

void MqttTask::callback_mqtt(char *topic, byte *payload, unsigned int length)
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

void MqttTask::reconnect_mqtt()
{
    while (!mqttClient.connected())
    {
        char buf_[64];
        sprintf(buf_, "Attempting connection %s %s %s", "smartknob", MQTT_USER, MQTT_PASSWORD);
        log(buf_);

        if (mqttClient.connect("smartknob", MQTT_USER, MQTT_PASSWORD))
        {
            sprintf(buf_, "Connected to %s", MQTT_SERVER);
            log(buf_);
        }
        else
        {
            sprintf(buf_, "Failed to connect to %s, retrying in 5s", MQTT_SERVER);
            log(buf_);
            delay(5000);
        }
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

#endif
