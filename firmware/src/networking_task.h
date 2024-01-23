#pragma once

#if SK_NETWORKING

#include <Arduino.h>
#include <WiFi.h>
#include <vector>
#include <map>
#include <mqtt.h>

#include "logger.h"
#include "proto_gen/smartknob.pb.h"
#include "task.h"
#include "app_config.h"
#include <WebServer.h>
#include <ElegantOTA.h>

class NetworkingTask : public Task<NetworkingTask>
{
    friend class Task<NetworkingTask>; // Allow base Task to invoke protected run()

public:
    NetworkingTask(const uint8_t task_core);
    ~NetworkingTask();

    QueueHandle_t getEntityStateReceivedQueue();

    void enqueueEntityStateToSend(EntityStateUpdate);

    void setLogger(Logger *logger);
    void addStateListener(QueueHandle_t queue);
    void addAppSyncListener(QueueHandle_t queue);

    void unlock();

    cJSON *getApps();

protected:
    void run();

private:
    QueueHandle_t entity_state_to_send_queue_;
    QueueHandle_t entity_state_received_queue_;

    std::vector<QueueHandle_t> state_listeners_;
    std::vector<QueueHandle_t> app_sync_listeners_;

    PB_SmartKnobState state_;
    SemaphoreHandle_t mutex_;
    SemaphoreHandle_t mutex_app_sync_;
    Logger *logger_;
    void log(const char *msg);
    WiFiClient wifi_client;
    void setup_wifi();
    void publishState(const ConnectivityState &state);
    void publishAppSync(const cJSON *state);
    void reconnect_mqtt();
    void mqtt_callback(char *topic, byte *payload, unsigned int length);
    PubSubClient mqttClient;

    char buf_[128];

    WebServer *server_;

    void lock();

    cJSON *apps;
};

#else

class NetworkingTask
{
};

#endif
