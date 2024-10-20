#if SK_MQTT
#include "mqtt_task.h"

static const char *MQTT_TAG = "MQTT";
MqttTask::MqttTask(const uint8_t task_core) : Task{"mqtt", 1024 * 15, 0, task_core}
{
    mutex_app_sync_ = xSemaphoreCreateMutex();

    entity_state_to_send_queue_ = xQueueCreate(50, sizeof(EntityStateUpdate));
    assert(entity_state_to_send_queue_ != NULL);

    mqtt_notifier = MqttNotifier();
    mqtt_notifier.setCallback([this](MqttCommand command)
                              { this->handleCommand(command); });
}

MqttTask::~MqttTask()
{
    vQueueDelete(entity_state_to_send_queue_);
    vSemaphoreDelete(mutex_app_sync_);
}

void MqttTask::setConfig(MQTTConfiguration config)
{
    config_ = config;
}

void MqttTask::handleCommand(MqttCommand command)
{
    switch (command.type)
    {
    case RequestSetupConnect:
        this->setupAndConnectNewCredentials(command.body.mqtt_config);
        break;
    case RequestConnect:
        this->setup(command.body.mqtt_config);
        break;
    default:
        break;
    }
}

void MqttTask::handleEvent(WiFiEvent event)
{

    switch (event.type)
    {
    case SK_MQTT_CONNECTED:
    case SK_MQTT_CONNECTED_NEW_CREDENTIALS:
        init();
        break;
    case SK_RESET_ERROR:
        retry_count = 0;
        LOGV(LOG_LEVEL_WARNING, "Resetting retry count");
        break;
    case SK_DISMISS_ERROR:
        break;
    default:
        break;
    }
}

void MqttTask::run()
{
    static uint32_t mqtt_pull;
    static uint32_t mqtt_push; // to prevent spam
    static uint32_t mqtt_init_interval;
    const uint16_t mqtt_pull_interval_ms = 200;
    const uint16_t mqtt_push_interval_ms = 200;

    static EntityStateUpdate entity_state_to_process_;

    static uint32_t last_mqtt_state_sent;

    char topic[64];
    sprintf(topic, "smartknob/%s/from_knob", WiFi.macAddress().c_str());

    static uint32_t message_count = 0;

    static bool has_been_connected = false;

    int disconnected_at;

    while (1)
    {
        if (is_config_set && retry_count < 3)
        {
            if (millis() - last_mqtt_state_sent > 1000 && !mqtt_client.connected() && WiFi.isConnected())
            {
                disconnected_at = millis();
                if (has_been_connected || retry_count > 0)
                {
                    WiFiEvent event;
                    WiFiEventBody wifi_event_body;
                    wifi_event_body.error.type = MQTT_ERROR;
                    wifi_event_body.error.body.mqtt_error.retry_count = retry_count + 1;
                    LOGE("MQTT connection failed, retrying...");
                    LOGE("Retry count: %d", retry_count + 1);

                    event.type = SK_MQTT_CONNECTION_FAILED;
                    event.body = wifi_event_body;
                    event.sent_at = millis();
                    publishEvent(event);
                }

                disconnect();

                if (!connect())
                {
                    delay(max(0, (int)(10000 - (millis() - disconnected_at)))); // REMOVE THIS DELAY?
                    retry_count++;
                    if (retry_count > 2)
                    {
                        LOGI("Retry limit reached...");
                        WiFiEvent event;
                        event.type = SK_MQTT_RETRY_LIMIT_REACHED;
                        publishEvent(event);
                    }
                    continue;
                }
                has_been_connected = true;
                if (retry_count > 0)
                {
                    WiFiEvent reset_error;
                    reset_error.type = SK_RESET_ERROR;
                    publishEvent(reset_error);
                }
                retry_count = 0;
            }

            if (millis() - mqtt_pull > mqtt_pull_interval_ms)
            {
                mqtt_client.loop();
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

                        sprintf(hexbuffer_, "%08lX", micros());
                        cJSON *json = cJSON_CreateObject();
                        cJSON_AddStringToObject(json, "id", hexbuffer_);
                        cJSON_AddStringToObject(json, "type", "state_update");
                        cJSON_AddStringToObject(json, "app_id", entity_states_to_send[i.first].app_id);
                        cJSON_AddRawToObject(json, "state", entity_states_to_send[i.first].state);

                        char *json_string = cJSON_PrintUnformatted(json);

                        unacknowledged_ids.insert(std::make_pair(hexbuffer_, "state_update"));
                        unacknowledged_states.insert(std::make_pair(hexbuffer_, entity_states_to_send[i.first]));
                        mqtt_client.publish(topic, json_string);

                        cJSON_free(json_string);
                        cJSON_Delete(json);

                        last_mqtt_state_sent = millis();
                        entity_states_to_send[i.first].sent = true;
                    }
                }

                mqtt_push = millis();
            }

            if (millis() - mqtt_init_interval > 10000 && !hass_init_acknowledged)
            {
                init();
                mqtt_init_interval = millis();
            }
        }

        mqtt_notifier.loopTick();
        delay(1);
    }
}

bool MqttTask::setup(MQTTConfiguration config)
{
    if (is_config_set)
    {
        reset();
    }

    WiFiEvent event;
    event.type = SK_MQTT_SETUP;
    sprintf(event.body.mqtt_connecting.host, "%s", config.host);
    event.body.mqtt_connecting.port = config.port;
    sprintf(event.body.mqtt_connecting.user, "%s", config.user);
    sprintf(event.body.mqtt_connecting.password, "%s", config.password);

    config_ = config;

    wifi_client.setTimeout(10);

    mqtt_client.setClient(wifi_client);
    mqtt_client.setServer(config_.host, config_.port);
    mqtt_client.setBufferSize(2048); // ADD BUFFER SIZE TO CONFIG? NO?
    mqtt_client.setCallback([this](char *topic, byte *payload, unsigned int length)
                            { this->callback(topic, payload, length); });

    publishEvent(event);
    is_config_set = true;
    return is_config_set;
}

bool MqttTask::setupAndConnectNewCredentials(MQTTConfiguration config)
{
    if (is_config_set)
    {
        reset();
    }

    WiFiEvent event;
    event.type = SK_MQTT_TRY_NEW_CREDENTIALS;
    event.body.mqtt_connecting = config;
    publishEvent(event);

    wifi_client.setTimeout(15); // 30s timeout didnt work, threw error (Software caused connection abort)

    mqtt_client.setClient(wifi_client);
    mqtt_client.setServer(config.host, config.port);
    mqtt_client.setBufferSize(SK_MQTT_BUFFER_SIZE); // ADD BUFFER SIZE TO CONFIG? NO?
    mqtt_client.setCallback([this](char *topic, byte *payload, unsigned int length)
                            { this->callback(topic, payload, length); });

    uint32_t timeout_at = millis() + 30000;

    disconnect();
    while (!mqtt_client.connect(config.knob_id, config.user, config.password) && timeout_at > millis())
    {
        delay(10); // DO NOTHING
    }

    if (mqtt_client.connected())
    {
        event.type = SK_MQTT_CONNECTED_NEW_CREDENTIALS;
        publishEvent(event);

        config_ = config;
        is_config_set = true;
        return true;
    }

    event.type = SK_MQTT_TRY_NEW_CREDENTIALS_FAILED;
    publishEvent(event);
    return false;
}

bool MqttTask::reset()
{
    WiFiEvent event;
    event.type = SK_MQTT_RESET;

    retry_count = 0;
    is_config_set = false;
    wifi_client.flush();
    mqtt_client.disconnect();
    publishEvent(event);

    return true;
}

bool MqttTask::connect()
{
    if (config_.host == "")
    {
        LOGD("No host set");
        return false;
    }

    bool mqtt_connected = false;
    if (config_.user == "")
    {
        LOGI("Connecting to MQTT without credentials");
        // TODO: Create and use knob id

        mqtt_connected = mqtt_client.connect(config_.knob_id);
    }
    else
    {
        LOGI("Connecting to MQTT with credentials");
        // TODO: Create and use knob id
        mqtt_connected = mqtt_client.connect(config_.knob_id, config_.user, config_.password);
    }

    if (mqtt_connected)
    {
        WiFiEvent event;
        event.type = SK_MQTT_CONNECTED;
        publishEvent(event);
        LOGI("MQTT client connected");
        return true;
    }
    else
    {
        WiFiEvent event;
        WiFiEventBody wifi_event_body;
        wifi_event_body.error.type = MQTT_ERROR;
        wifi_event_body.error.body.mqtt_error.retry_count = retry_count + 1;

        event.type = SK_MQTT_CONNECTION_FAILED;
        event.body = wifi_event_body;
        event.sent_at = millis();

        LOGE("MQTT connection failed");
    }

    return false;
}

bool MqttTask::disconnect()
{
    wifi_client.flush();
    mqtt_client.disconnect();
    mqtt_client.flush();
    mqtt_client.loop();
    return true;
}

bool MqttTask::init()
{
    char buf_[128];
    sprintf(buf_, "smartknob/%s/from_hass", WiFi.macAddress().c_str());
    mqtt_client.subscribe(buf_);

    sprintf(hexbuffer_, "%08lX", millis());

    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "id", hexbuffer_);
    cJSON_AddStringToObject(json, "mac_address", WiFi.macAddress().c_str());

    cJSON *data = cJSON_CreateObject();

    cJSON_AddStringToObject(data, "model", MODEL);
#ifdef RELEASE_VERSION
    cJSON_AddStringToObject(data, "firmware_version", RELEASE_VERSION);
#else
    cJSON_AddStringToObject(data, "firmware_version", "DEV");
#endif
    cJSON_AddStringToObject(data, "manufacturer", "Seedlabs");

    cJSON_AddItemToObject(json, "data", data);

    unacknowledged_ids.insert(std::make_pair(hexbuffer_, "init"));

    char *init_string = cJSON_PrintUnformatted(json);
    mqtt_client.publish("smartknob/init", init_string);

    cJSON_free(init_string);
    cJSON_Delete(json);

    mqtt_client.loop();

    WiFiEvent mqtt_connected_event;
    mqtt_connected_event.type = SK_MQTT_INIT;
    publishEvent(mqtt_connected_event);
    return true;
}

void MqttTask::callback(char *topic, byte *payload, unsigned int length)
{
    cJSON *json_root = cJSON_Parse((char *)payload);
    cJSON *type = cJSON_GetObjectItem(json_root, "type");

    if (type == NULL)
    {
        LOGW("Invalid message received");
        return;
    }

    char buf_[128];
    sprintf(buf_, "smartknob/%s/from_knob", WiFi.macAddress().c_str());

    if (strcmp(type->valuestring, "sync") == 0)
    {
        cJSON *json_root_ = cJSON_Parse((char *)payload);

        lock();
        apps = cJSON_GetObjectItem(json_root_, "apps"); //! THIS APPS OBJECT NEEDS TO BE FIXED!!! WAS CAUSING MEMORY LEAK BEFORE WHEN USING json_root instead of json_root_
        if (apps == NULL)
        {
            LOGW("Invalid message received");
            return;
        }
        unlock();

        // DELAY TO MAKE SURE APPS ARE INITIALIZED?
        delay(100);

        cJSON *json = cJSON_CreateObject();
        cJSON_AddStringToObject(json, "type", "acknowledgement");
        cJSON_AddStringToObject(json, "data", "sync");

        char *acknowledgement_payload = cJSON_PrintUnformatted(json);
        mqtt_client.publish(buf_, acknowledgement_payload);

        cJSON_free(acknowledgement_payload);
        cJSON_Delete(json);

        publishAppSync(apps);
    }

    if (strcmp(type->valuestring, "state_update") == 0)
    {
        LOGV(LOG_LEVEL_DEBUG, "State_update received");

        cJSON *app_id = cJSON_GetObjectItem(json_root, "app_id");
        cJSON *entity_id = cJSON_GetObjectItem(json_root, "entity_id");
        cJSON *new_state = cJSON_GetObjectItem(json_root, "new_state");

        if (app_id == NULL || entity_id == NULL || new_state == NULL)
        {
            LOGW("Invalid message received");
            return;
        }

        MQTTStateUpdate state_update;
        state_update.all = false;
        sprintf(state_update.app_id, "%s", app_id->valuestring);
        sprintf(state_update.entity_id, "%s", entity_id->valuestring);
        sprintf(state_update.state, "%s", "");

        char *state_string = cJSON_PrintUnformatted(new_state);
        sprintf(state_update.state, "%s", state_string);

        cJSON_free(state_string);

        WiFiEvent event;
        event.type = SK_MQTT_STATE_UPDATE;
        event.body.mqtt_state_update = state_update;

        publishEvent(event);
    }

    if (strcmp(type->valuestring, "acknowledgement") == 0)
    {
        cJSON *acknowledge_id = cJSON_GetObjectItem(json_root, "acknowledge_id");
        cJSON *acknowledge_type = cJSON_GetObjectItem(json_root, "acknowledge_type");

        if (acknowledge_id == NULL || acknowledge_type == NULL)
        {
            LOGW("Invalid message received");
            return;
        }

        if (unacknowledged_ids.find(acknowledge_id->valuestring) != unacknowledged_ids.end())
        {
            unacknowledged_ids.erase(acknowledge_id->valuestring);

            if (strcmp(acknowledge_type->valuestring, "init") == 0)
            {
                hass_init_acknowledged = true;
            }

            else if (strcmp(acknowledge_type->valuestring, "state_update") == 0)
            {
                if (unacknowledged_states.find(acknowledge_id->valuestring) != unacknowledged_states.end())
                {
                    EntityStateUpdate state = unacknowledged_states[acknowledge_id->valuestring];

                    MQTTStateUpdate state_update;
                    state_update.all = true;
                    sprintf(state_update.app_id, "%s", state.app_id);
                    sprintf(state_update.entity_id, "%s", state.entity_id);
                    sprintf(state_update.state, "%s", state.state);

                    WiFiEvent event;
                    event.type = SK_MQTT_STATE_UPDATE;
                    event.body.mqtt_state_update = state_update;

                    publishEvent(event);

                    unacknowledged_states.erase(acknowledge_id->valuestring);
                }
            }
        }
    }

    cJSON_Delete(json_root);
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

MqttNotifier *MqttTask::getNotifier()
{
    return &mqtt_notifier;
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
    event.sent_at = millis();
    xQueueSendToBack(shared_events_queue, &event, 0);
}

#endif
