#include "spotify_task.h"

SpotifyTask::SpotifyTask(const uint8_t task_core, Configuration &configuration) : Task{"spotify", 1024 * 12, 1, task_core}, spotify_api_(configuration)
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
        if (WiFi.status() == WL_CONNECTED) // TODO check spotify availability
        {
            unsigned long ms_since_last_fetch = millis() - last_fetched_playback_state;

            if (last_fetched_playback_state == 0 || millis() - last_fetched_playback_state > playback_state_fetch_interval || (latest_playback_state_.available && ms_since_last_fetch + latest_playback_state_.progress_ms > latest_playback_state_.item.duration_ms))
            {
                PlaybackState playback_state = spotify_api_.getCurrentPlaybackState();
                LOGE("Fetching playback state");

                last_fetched_playback_state = millis();
                ms_since_last_fetch = millis() - last_fetched_playback_state;

                WiFiEvent playback_state_event;
                playback_state_event.type = SK_SPOTIFY_PLAYBACK_STATE;
                playback_state_event.body.playback_state = playback_state;

                publishEvent(playback_state_event);

                if (playback_state.available && (latest_playback_state_.timestamp == 0 || strcmp(playback_state.item.name, latest_playback_state_.item.name) != 0))
                {
                    delay(50); // TODO Look into if this helped preventing failure of image fetch!?
                    LOGE("Fetching cover art");
                    if (strcmp(playback_state.item.album.images[1].url, "") != 0)
                    {
                        spotify_api_.downloadImage(playback_state.item.album.images[1].url);
                    }
                    else
                    {
                        LOGE("No image found");
                        return; // TODO handle
                    }

                    if (spotify_api_.imageSize == 0)
                    {
                        LOGE("No image found");
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

                latest_playback_state_ = playback_state;
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
        if (spotify_api_.setVolume(event.body.volume, device_id))
        {
            latest_playback_state_.device.volume_percent = event.body.volume;
            playback_state_event.body.playback_state = latest_playback_state_;
        }
        return;
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
    event.sent_at = millis();
    xQueueSendToBack(shared_events_queue_, &event, 0);
}