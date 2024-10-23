
#include "spotify_api.h"

bool SpotifyApi::isPlaying()
{
    checkAndRefreshToken();
    return getCurrentPlaybackState().is_playing;
    // return false;
}

PlaybackState SpotifyApi::getCurrentPlaybackState()
{
    const unsigned long current_ms = millis();

    if (latest_playback_state_.timestamp > 0 && latest_playback_state_.progress_ms + current_ms <= latest_playback_state_.item.duration_ms)
    {
        if (latest_playback_state_.is_playing)
        {
            LOGE("Updating progress")
            latest_playback_state_.progress_ms += current_ms;
        }
        LOGE("Not fetching new state but updating.")
        return latest_playback_state_;
    }

    LOGE("Fetching new state")

    PlaybackState playback_state_;
    playback_state_.available = false;

    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Bearer %s", spotify_config_.access_token);

    HTTPClient http;
    http.setTimeout(1000);
    String url = "https://api.spotify.com/v1/me/player";
    http.begin(url);
    http.addHeader("Authorization", auth_header);

    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        String payload = http.getString();

        cJSON *json = cJSON_Parse(payload.c_str());
        if (json == NULL)
        {
            LOGE("Error parsing JSON");
            http.end();
            playback_state_.available = false;
            return latest_playback_state_;
        }

        strcpy(playback_state_.repeat_state, cJSON_GetObjectItem(json, "repeat_state")->valuestring);
        playback_state_.shuffle_state = cJSON_GetObjectItem(json, "shuffle_state")->valueint;
        playback_state_.timestamp = cJSON_GetObjectItem(json, "timestamp")->valuedouble;
        playback_state_.progress_ms = cJSON_GetObjectItem(json, "progress_ms")->valueint;
        playback_state_.is_playing = cJSON_GetObjectItem(json, "is_playing")->valueint;
        strcpy(playback_state_.currently_playing_type, cJSON_GetObjectItem(json, "currently_playing_type")->valuestring);

        cJSON *device = cJSON_GetObjectItem(json, "device");
        if (device == NULL)
        {
            LOGE("No device object found in JSON");
            cJSON_Delete(json);
            http.end();
            return latest_playback_state_;
        }
        else
        {
            strcpy(playback_state_.device.id, cJSON_GetObjectItem(device, "id")->valuestring);
            playback_state_.device.is_active = cJSON_GetObjectItem(device, "is_active")->valueint;
            playback_state_.device.is_private_session = cJSON_GetObjectItem(device, "is_private_session")->valueint;
            playback_state_.device.is_restricted = cJSON_GetObjectItem(device, "is_restricted")->valueint;
            strcpy(playback_state_.device.name, cJSON_GetObjectItem(device, "name")->valuestring);
            strcpy(playback_state_.device.type, cJSON_GetObjectItem(device, "type")->valuestring);
            playback_state_.device.volume_percent = cJSON_GetObjectItem(device, "volume_percent")->valueint;
            playback_state_.device.supports_volume = cJSON_GetObjectItem(device, "supports_volume")->valueint;
        }

        cJSON *item = cJSON_GetObjectItem(json, "item");
        if (item == NULL)
        {
            LOGE("No item object found in JSON");
            cJSON_Delete(json);
            http.end();
            return latest_playback_state_;
        }
        else
        {
            cJSON *album = cJSON_GetObjectItem(item, "album");
            if (album == NULL)
            {
                LOGE("No album object found in JSON");
                cJSON_Delete(json);
                http.end();
                return latest_playback_state_;
            }
            else
            {
                cJSON *images = cJSON_GetObjectItem(album, "images");
                if (images == NULL)
                {
                    LOGE("No images object found in JSON");
                    cJSON_Delete(json);
                    http.end();
                    return playback_state_;
                }
                else
                {
                    for (int i = 0; i < cJSON_GetArraySize(images); i++)
                    {
                        cJSON *image = cJSON_GetArrayItem(images, i);
                        if (image == NULL)
                        {
                            LOGE("No image object found in JSON");
                            cJSON_Delete(json);
                            http.end();
                            return playback_state_;
                        }
                        else
                        {
                            strcpy(playback_state_.item.album.images[i].url, cJSON_GetObjectItem(image, "url")->valuestring);
                            playback_state_.item.album.images[i].width = cJSON_GetObjectItem(image, "width")->valueint;
                            playback_state_.item.album.images[i].height = cJSON_GetObjectItem(image, "height")->valueint;
                        }
                    }
                }

                strcpy(playback_state_.item.album.name, cJSON_GetObjectItem(album, "name")->valuestring);
            }

            playback_state_.item.duration_ms = cJSON_GetObjectItem(item, "duration_ms")->valueint;
            strcpy(playback_state_.item.name, cJSON_GetObjectItem(item, "name")->valuestring);
            LOGE("Track name: %s", playback_state_.item.name);
        }

        cJSON *actions = cJSON_GetObjectItem(json, "actions");
        if (actions == NULL)
        {
            LOGE("No actions object found in JSON");
            cJSON_Delete(json);
            http.end();
            // return playback_state_;
        }
        else
        {
            // TODO optional field from api
            // playback_state_.actions.interrupting_playback = cJSON_GetObjectItem(actions, "interrupting_playback")->valueint;
            // playback_state_.actions.pausing = cJSON_GetObjectItem(actions, "pausing")->valueint;
            // playback_state_.actions.resuming = cJSON_GetObjectItem(actions, "resuming")->valueint;
            // playback_state_.actions.seeking = cJSON_GetObjectItem(actions, "seeking")->valueint;
            // playback_state_.actions.skipping_next = cJSON_GetObjectItem(actions, "skipping_next")->valueint;
            // playback_state_.actions.skipping_prev = cJSON_GetObjectItem(actions, "skipping_prev")->valueint;
            // playback_state_.actions.toggling_repeat_context = cJSON_GetObjectItem(actions, "toggling_repeat_context")->valueint;
            // playback_state_.actions.toggling_shuffle = cJSON_GetObjectItem(actions, "toggling_shuffle")->valueint;
            // playback_state_.actions.toggling_repeat_track = cJSON_GetObjectItem(actions, "toggling_repeat_track")->valueint;
            // playback_state_.actions.transferring_playback = cJSON_GetObjectItem(actions, "transferring_playback")->valueint;
        }

        cJSON_Delete(json); // Free memory
    }
    else if (httpCode == HTTP_CODE_NO_CONTENT)
    {
        LOGE("No content");

        playback_state_.spotify_available = true;
        return latest_playback_state_;
    }
    else
    {
        LOGE("Error in HTTP request: %d", httpCode);
        return latest_playback_state_;
    }
    http.end();
    playback_state_.available = true;
    latest_playback_state_ = playback_state_;
    last_fetched_state_ms_ = millis();
    return playback_state_;
}

void SpotifyApi::refreshToken()
{
    HTTPClient http;
    http.setTimeout(1000);
    http.begin("https://accounts.spotify.com/api/token");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    char auth_header[256];
    snprintf(auth_header, sizeof(auth_header), "Basic %s", spotify_config_.base64_id_and_secret);
    http.addHeader("Authorization", auth_header);

    char body[512];
    snprintf(body, sizeof(body), "grant_type=refresh_token&refresh_token=%s", spotify_config_.refresh_token);
    http.addHeader("Content-Length", String(strlen(body)));

    int httpCode = http.POST(body);

    if (httpCode == HTTP_CODE_OK)
    {
        String payload = http.getString();

        cJSON *json = cJSON_Parse(payload.c_str());

        if (json == NULL)
        {
            LOGE("Error parsing JSON"); // TODO handle
            http.end();
            return;
        }

        strcpy(spotify_config_.access_token, cJSON_GetObjectItem(json, "access_token")->valuestring);
        strcpy(spotify_config_.token_type, cJSON_GetObjectItem(json, "token_type")->valuestring);
        spotify_config_.expires_in = cJSON_GetObjectItem(json, "expires_in")->valueint;
        if (cJSON_HasObjectItem(json, "refresh_token"))
        {
            strcpy(spotify_config_.refresh_token, cJSON_GetObjectItem(json, "refresh_token")->valuestring);
        }
        strcpy(spotify_config_.scope, cJSON_GetObjectItem(json, "scope")->valuestring);

        WiFiEvent wifi_event = {
            .type = SK_SPOTIFY_ACCESS_TOKEN_RECEIVED,
            .body = {
                .spotify_config = spotify_config_}};
        publishEvent(wifi_event);
    }
    else
    {
        LOGE("Error in HTTP request: %d", httpCode);
    }
    http.end();
}

bool SpotifyApi::checkAndRefreshToken()
{
    if (last_refreshed_ms_ == 0 || millis() >= last_refreshed_ms_ + spotify_config_.expires_in * 10) // 10x for testing 3.6mins // seems to be working but mqtt and wifi having problems....
    {
        // getCurrentPlaybackState();

        // LOGE("Token expired, refreshing");
        // return refreshToken()
        refreshToken();                // TODO disable until we store the new token
        last_refreshed_ms_ = millis(); // 10x for testing 3.6mins
        LOGE("Token refreshed");
    }
    // return getCurrentPlaybackState();
    delay(50);
    return true;
}

bool SpotifyApi::play()
{
    checkAndRefreshToken();
    int16_t result = sendPutRequest("https://api.spotify.com/v1/me/player/play?device_id" + String(latest_playback_state_.device.id ? latest_playback_state_.device.id : ""));
    if (result == HTTP_CODE_OK || result == HTTP_CODE_NO_CONTENT)
    {
        latest_playback_state_.is_playing = !latest_playback_state_.is_playing;
        return true;
    }
    LOGE("Error in HTTP request: %d", result);
    return false;
}

bool SpotifyApi::pause()
{
    checkAndRefreshToken();
    int16_t result = sendPutRequest("https://api.spotify.com/v1/me/player/pause?device_id" + String(latest_playback_state_.device.id ? latest_playback_state_.device.id : ""));
    if (result == HTTP_CODE_OK || result == HTTP_CODE_NO_CONTENT)
    {
        latest_playback_state_.is_playing = !latest_playback_state_.is_playing;
        return true;
    }
    LOGE("Error in HTTP request: %d", result);
    return false;
}

int16_t SpotifyApi::sendPutRequest(const String &url, const String &body)
{
    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Bearer %s", spotify_config_.access_token);

    HTTPClient http;
    http.setTimeout(1000);
    http.begin(url);
    http.addHeader("Authorization", auth_header);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Content-Length", String(body.length()));

    int httpCode = http.PUT(body);

    // if (httpCode == HTTP_CODE_OK)
    // {
    //     LOGE("Request successful");
    //     return true;
    // }
    // else if (httpCode == HTTP_CODE_NO_CONTENT)
    // {
    //     LOGE("No content"); // TODO tell user that the request was successful but no content was returned, ie no device is playing anything
    // }
    // else
    // {
    //     LOGE("Error in HTTP request: %d", httpCode);
    // }
    http.end();
    return httpCode;
}

void SpotifyApi::setConfig(const PB_SpotifyConfig &spotify_config)
{
    spotify_config_ = spotify_config;
}

PB_SpotifyConfig SpotifyApi::getConfig()
{
    return spotify_config_;
}

bool SpotifyApi::isAvailable()
{
    if (spotify_config_.expires_in > 0)
        return checkAndRefreshToken();
    return false;
}

void SpotifyApi::downloadImage()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        char imageUrl[128] = "";
        PlaybackState playback_state = getCurrentPlaybackState();
        if (!playback_state.available)
        {
            LOGW("Playback state not available");
            return;
        }

        for (int i = 0; i < MAX_IMAGES; i++)
        {
            if (playback_state.item.album.images[i].height == 300)
            {
                strcpy(imageUrl, playback_state.item.album.images[i].url);
                break;
            }
        }

        if (strlen(imageUrl) == 0)
        {
            LOGE("No image found");
            return;
        }

        LOGE("Downloading image from: %s", imageUrl);

        HTTPClient http;
        http.setTimeout(1000);
        http.begin(imageUrl);

        int httpCode = http.GET();
        if (httpCode == HTTP_CODE_OK)
        {
            imageSize = http.getSize();
            imageBuffer = (uint8_t *)heap_caps_aligned_alloc(16, imageSize, MALLOC_CAP_SPIRAM);

            if (imageBuffer != nullptr)
            {
                WiFiClient *stream = http.getStreamPtr();
                size_t offset = 0;
                uint8_t buff[256];

                while (http.connected() && (stream->available() > 0) && (offset < imageSize))
                {
                    int bytesRead = stream->readBytes(buff, sizeof(buff));
                    if (bytesRead > 0)
                    {
                        memcpy(imageBuffer + offset, buff, bytesRead);
                        offset += bytesRead;
                    }
                }

                LOGV(LOG_LEVEL_DEBUG, "Image downloaded and stored in PSRAM");
            }
            else
            {
                LOGE("Failed to allocate PSRAM for image.");
            }
        }
        else
        {
            LOGE("Error in HTTP request: %d", httpCode);
        }

        http.end();
    }
}

String SpotifyApi::getCurrentTrackName()
{
    checkAndRefreshToken();
    return getCurrentPlaybackState().item.name;
}

void SpotifyApi::setSharedEventsQueue(QueueHandle_t shared_events_queue)
{
    this->shared_events_queue = shared_events_queue;
}

void SpotifyApi::publishEvent(WiFiEvent event)
{
    event.sent_at = millis();
    xQueueSendToBack(shared_events_queue, &event, 0);
}