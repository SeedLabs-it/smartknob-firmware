#include "spotify_task.h"

SpotifyTask::SpotifyTask(const uint8_t task_core, Configuration &configuration) : Task{"spotify", 1024 * 10, 1, task_core}, spotify_api_(configuration)
{
}

SpotifyTask::~SpotifyTask()
{
}

void SpotifyTask::run()
{
    static uint16_t playback_state_fetch_interval = 20000;
    static unsigned long last_fetched_playback_state = 0;

    while (1)
    {
        if (WiFi.status() == WL_CONNECTED && shared_events_queue_ != nullptr) // TODO check spotify availability
        {
            unsigned long ms_since_last_fetch = millis() - last_fetched_playback_state;

            if (last_fetched_playback_state == 0 || millis() - last_fetched_playback_state > playback_state_fetch_interval || (latest_playback_state_.available && ms_since_last_fetch + latest_playback_state_.progress_ms > latest_playback_state_.item.duration_ms))
            {
                delay(50); // TODO Look into if this helped preventing failure of image fetch!?
                PlaybackState playback_state = spotify_api_.getCurrentPlaybackState();

                last_fetched_playback_state = millis();
                ms_since_last_fetch = millis() - last_fetched_playback_state;

                WiFiEvent playback_state_event;
                playback_state_event.type = SK_SPOTIFY_PLAYBACK_STATE;
                playback_state_event.body.playback_state = playback_state;

                publishEvent(playback_state_event);

                if (playback_state.available)
                {
                    if (latest_playback_state_.timestamp == 0 || strcmp(playback_state.item.name, latest_playback_state_.item.name) != 0)
                    {
                        delay(50); // TODO Look into if this helped preventing failure of image fetch!?
                        if (strcmp(playback_state.item.album.images[1].url, "") != 0)
                        {
                            LOGV(LOG_LEVEL_DEBUG, "Fetching spotify cover art from: %s", playback_state.item.album.images[1].url);
                            spotify_api_.downloadImage(playback_state.item.album.images[1].url);
                        }
                        else
                        {
                            LOGE("No cover art found");
                            return; // TODO handle
                        }

                        if (spotify_api_.imageSize == 0)
                        {
                            LOGE("No cover art found");
                            return; // TODO handle
                        }

                        image_dsc = (lv_img_dsc_t *)heap_caps_malloc(sizeof(lv_img_dsc_t), MALLOC_CAP_SPIRAM);

                        image_dsc->header.always_zero = 0;
                        image_dsc->header.w = 300;
                        image_dsc->header.h = 300;
                        image_dsc->header.cf = LV_IMG_CF_TRUE_COLOR;
                        image_dsc->data = spotify_api_.imageBuffer;
                        image_dsc->data_size = spotify_api_.imageSize;

                        WiFiEvent cover_art_event;
                        cover_art_event.type = SK_SPOTIFY_NEW_COVER_ART;
                        cover_art_event.body.cover_art = image_dsc;
                        publishEvent(cover_art_event);
                    }

                    LOGE("DEVICE_ID: %s", playback_state.device.id);

                    if (strcmp(playback_state.device.id, "") != 0 && strcmp(playback_state.device.id, latest_playback_state_.device.id) != 0)
                    {
                        LOGE("Device changed");
                        WiFiEvent device_changed_event = {
                            .type = SK_SPOTIFY_DEVICE_CHANGED,
                        };

                        strcpy(device_changed_event.body.spotify_device_id, playback_state.device.id);

                        publishEvent(device_changed_event);
                    }
                }

                latest_playback_state_ = playback_state;
            }

            if (latest_playback_state_.spotify_available)
            {
                if (millis() - last_volume_change_ms > 200 && last_volume != latest_playback_state_.device.volume_percent && last_volume != 255)
                {
                    last_volume_change_ms = millis();
                    LOGV(LOG_LEVEL_DEBUG, "Adjusting Spotify volume");
                    if (spotify_api_.setVolume(last_volume, latest_playback_state_.device.id))
                    {
                        LOGV(LOG_LEVEL_DEBUG, "Spotify volume set to: %d", last_volume);
                        latest_playback_state_.device.volume_percent = last_volume;
                    }
                    else
                    {
                        LOGW("Failed to set Spotify volume");
                    }
                }
            }
        }
        delay(1);
    }
}

void SpotifyTask::handleEvent(const WiFiEvent &event)
{
    WiFiEvent playback_state_event;
    playback_state_event.type = SK_SPOTIFY_PLAYBACK_STATE;
    playback_state_event.body.playback_state = latest_playback_state_;

    const char *device_id = latest_playback_state_.device.id ? latest_playback_state_.device.id : "";

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
        if (event.body.volume != last_volume && latest_playback_state_.device.volume_percent != event.body.volume)
        {
            last_volume = event.body.volume;
        }
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