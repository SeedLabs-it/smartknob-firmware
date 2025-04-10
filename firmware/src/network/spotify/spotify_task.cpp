#include "spotify_task.h"

SpotifyTask::SpotifyTask(const uint8_t task_core, Configuration &configuration)
    : Task{"spotify", 1024 * 10, 1, task_core}, spotify_api_(configuration)
{
    mutex_ = xSemaphoreCreateMutex();
    assert(mutex_ != nullptr);
}

SpotifyTask::~SpotifyTask() {}

void SpotifyTask::run()
{
    static uint16_t playback_state_fetch_interval = 5000;
    static unsigned long last_fetched_playback_state = 0;

    while (1)
    {
        if (spotify_api_.hasConfig() && WiFi.status() == WL_CONNECTED &&
            shared_events_queue_ != nullptr) // TODO check spotify availability
        {
            unsigned long ms_since_last_fetch =
                millis() - last_fetched_playback_state;
            if (state_invalidated || last_fetched_playback_state == 0 ||
                millis() - last_fetched_playback_state >
                    playback_state_fetch_interval ||
                (latest_playback_state_.available &&
                 ms_since_last_fetch + latest_playback_state_.progress_ms >
                     latest_playback_state_.item.duration_ms))
            {
                SemaphoreGuard lock(mutex_);
                state_invalidated = false;
                PlaybackState playback_state = spotify_api_.getCurrentPlaybackState();

                last_fetched_playback_state = millis();

                WiFiEvent playback_state_event;
                playback_state_event.type = SK_SPOTIFY_PLAYBACK_STATE;
                playback_state_event.body.playback_state = playback_state;

                publishEvent(playback_state_event);

                if (playback_state.available)
                {
                    if (latest_playback_state_.timestamp == 0 ||
                        strcmp(playback_state.item.name,
                               latest_playback_state_.item.name) != 0)
                    {
                        delay(50); // TODO Look into if this helped preventing failure of
                        if (strcmp(playback_state.item.album.images[1].url, "") != 0)
                        {
                            char image_path[512];
                            snprintf(image_path, sizeof(image_path),
                                     "/backend/v1/cover_art.php?url=%s", playback_state.item.album.images[1].url);

                            LOGV(LOG_LEVEL_DEBUG, "Fetching spotify cover art from: https://configurator.seedlabs.it%s", image_path);

                            spotify_api_.downloadImage(image_path);

                            char colors_path[512];
                            snprintf(colors_path, sizeof(colors_path), "/backend/v1/cover_art_colors_extraction.php?url=%s", playback_state.item.album.images[1].url);

                            LOGV(LOG_LEVEL_DEBUG, "Fetching spotify cover art colors from: https://configurator.seedlabs.it%s", colors_path);
                            spotify_api_.fetchImageColors(colors_path);
                        }
                        else
                        {
                            LOGE("No cover art found");
                            continue; // TODO handle!!!!!!!!!
                        }
                    }

                    if (strcmp(playback_state.device.id, "") != 0 &&
                        strcmp(playback_state.device.id,
                               latest_playback_state_.device.id) != 0 &&
                        strcmp(spotify_api_.getDeviceId(), playback_state.device.id) != 0)
                    {
                        WiFiEvent device_changed_event = {
                            .type = SK_SPOTIFY_DEVICE_CHANGED,
                        };

                        strcpy(device_changed_event.body.spotify_device_id,
                               playback_state.device.id);

                        publishEvent(device_changed_event);
                    }
                }

                latest_playback_state_ = playback_state;
            }

            if (latest_playback_state_.spotify_available)
            {
                // last_volume != latest_playback_state_.device.vol &&
                if (millis() - last_volume_change_ms > 500 &&
                    last_volume != 255)
                {
                    SemaphoreGuard lock(mutex_);
                    last_volume_change_ms = millis();
                    LOGV(LOG_LEVEL_DEBUG, "Adjusting Spotify volume on device: %s", latest_playback_state_.device.id);
                    if (spotify_api_.setVolume(last_volume,
                                               latest_playback_state_.device.id))
                    {
                        LOGV(LOG_LEVEL_DEBUG, "Spotify volume set to: %d", last_volume);
                        latest_playback_state_.device.volume_percent = last_volume;
                    }
                    else
                    {
                        last_volume = latest_playback_state_.device.volume_percent;
                        LOGW("Failed to set Spotify volume");
                    }
                    last_volume = 255;
                }
            }
        }
        delay(1);
    }
}

void SpotifyTask::handleEvent(const WiFiEvent &event)
{
    SemaphoreGuard lock(mutex_);
    WiFiEvent playback_state_event;
    playback_state_event.type = SK_SPOTIFY_PLAYBACK_STATE;
    playback_state_event.body.playback_state = latest_playback_state_;

    const char *device_id =
        latest_playback_state_.device.id ? latest_playback_state_.device.id : "";
    if (strcmp(latest_playback_state_.device.id, "") == 0)
    {
        state_invalidated = true;
    }
    switch (event.type)
    {
    case SK_SPOTIFY_PLAY:
        if (spotify_api_.play(device_id))
        {
            latest_playback_state_.is_playing = !latest_playback_state_.is_playing;
            playback_state_event.body.playback_state = latest_playback_state_;
        }
        break;
    case SK_SPOTIFY_PAUSE:
        if (spotify_api_.pause(device_id))
        {
            latest_playback_state_.is_playing = !latest_playback_state_.is_playing;
            playback_state_event.body.playback_state = latest_playback_state_;
        }
        break;
    case SK_SPOTIFY_VOLUME:
        if (event.body.volume != last_volume &&
            latest_playback_state_.device.volume_percent != event.body.volume && !first_run)
        {
            last_volume = event.body.volume;
        }
        first_run = false;
        break;
    default:
        break;
    }

    publishEvent(playback_state_event);
}

void SpotifyTask::setConfig(const PB_SpotifyConfig &spotify_config)
{
    spotify_api_.setConfig(spotify_config); // Handle differently
}

void SpotifyTask::setSharedEventsQueue(QueueHandle_t shared_events_queue)
{
    shared_events_queue_ = shared_events_queue;
    spotify_api_.setSharedEventsQueue(shared_events_queue);
}

void SpotifyTask::publishEvent(WiFiEvent event)
{
    // if (shared_events_queue_ == NULL)
    // {
    //     LOGE("No shared events queue set");
    //     return; // TODO handle!!!!!!!!!
    // }
    event.sent_at = millis();
    xQueueSendToBack(shared_events_queue_, &event, 0);
}
