#pragma once
#include "../app_config.h"
#include "../events/events.h"
#include "Arduino.h"
#include "cJSON.h"
#include "spotify_api.h"
#include "task.h"
#include <WiFi.h>
#include <draw/lv_img_buf.h>
#include "semaphore_guard.h"

class SpotifyTask : public Task<SpotifyTask>
{
    friend class Task<SpotifyTask>;

public:
    SpotifyTask(const uint8_t task_core, Configuration &configuration);
    ~SpotifyTask();

    void setConfig(const PB_SpotifyConfig &spotify_config);

    void handleEvent(const WiFiEvent &event);
    void setSharedEventsQueue(QueueHandle_t shared_events_queue);

protected:
    void run();

private:
    SemaphoreHandle_t mutex_ = nullptr;
    unsigned long last_volume_change_ms = 0;
    uint8_t last_volume = 255;

    SpotifyApi spotify_api_;

    lv_img_dsc_t *image_dsc = nullptr;

    PlaybackState latest_playback_state_{
        .available = false,
        .spotify_available = false,
        .timestamp = 0,
        .progress_ms = 0,
    };

    boolean state_invalidated = false;

    QueueHandle_t shared_events_queue_;
    void publishEvent(WiFiEvent event);
};
