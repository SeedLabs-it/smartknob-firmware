
#include "spotify_api.h"

SpotifyApi::SpotifyApi(Configuration &configuration) : configuration_(configuration)
{
    auth_header_ = (char *)heap_caps_calloc(1, AUTH_HEADER_SIZE, MALLOC_CAP_SPIRAM);
    assert(auth_header_ != nullptr);

    refresh_token_body_ = (char *)heap_caps_calloc(1, REFRESH_TOKEN_SIZE, MALLOC_CAP_SPIRAM);
    assert(refresh_token_body_ != nullptr);

    base64_basic_auth_header_ = (char *)heap_caps_calloc(1, BASE64_BASIC_AUTH_HEADER_SIZE, MALLOC_CAP_SPIRAM);
    assert(base64_basic_auth_header_ != nullptr);
}

SpotifyApi::~SpotifyApi()
{
    if (auth_header_ != nullptr)
    {
        heap_caps_free(auth_header_);
    }

    if (refresh_token_body_ != nullptr)
    {
        heap_caps_free(refresh_token_body_);
    }

    if (base64_basic_auth_header_ != nullptr)
    {
        heap_caps_free(base64_basic_auth_header_);
    }
}

bool SpotifyApi::isPlaying()
{
    return getCurrentPlaybackState().is_playing;
    // return false;
}

PlaybackState SpotifyApi::getCurrentPlaybackState(bool override)
{
    PlaybackState playback_state{.available = false, .spotify_available = false, .timestamp = 0, .progress_ms = 0};
    if (!checkAndRefreshToken())
    {
        LOGE("Failed to refresh token");
        playback_state.available = false;
        playback_state.spotify_available = false;
        return playback_state;
    }

    String url = "https://api.spotify.com/v1/me/player";
    http_client_.begin(url);
    http_client_.addHeader("Authorization", auth_header_);

    int httpCode = http_client_.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        int content_length = http_client_.getSize();
        char *buffer_ = (char *)heap_caps_calloc(1, content_length, MALLOC_CAP_SPIRAM);
        if (buffer_ == nullptr)
        {
            LOGE("Failed to allocate memory for buffer");
            http_client_.end();
            playback_state.available = false;
            return playback_state;
        }

        WiFiClient *stream = http_client_.getStreamPtr();
        uint16_t total_bytes_read = 0;
        while (http_client_.connected() && (total_bytes_read < content_length))
        {
            // Read data in chunks and process
            size_t available_data = stream->available();
            if (available_data)
            {
                char buffer[128]; // Example buffer size
                uint16_t bytes_read = stream->readBytes(buffer, min(available_data, sizeof(buffer)));
                memcpy(buffer_ + total_bytes_read, buffer, bytes_read);

                assert(total_bytes_read + bytes_read <= content_length);

                total_bytes_read += bytes_read;
            }
        }

        cJSON
            *json = cJSON_Parse(buffer_);

        heap_caps_free(buffer_);

        if (json == NULL)
        {
            LOGE("Error parsing JSON");
            http_client_.end();
            playback_state.available = false;
            return playback_state;
        }

        strcpy(playback_state.repeat_state, cJSON_GetObjectItem(json, "repeat_state")->valuestring);
        playback_state.shuffle_state = cJSON_GetObjectItem(json, "shuffle_state")->valueint;
        playback_state.timestamp = cJSON_GetObjectItem(json, "timestamp")->valuedouble;
        playback_state.progress_ms = cJSON_GetObjectItem(json, "progress_ms")->valueint;
        playback_state.is_playing = cJSON_GetObjectItem(json, "is_playing")->valueint;
        strcpy(playback_state.currently_playing_type, cJSON_GetObjectItem(json, "currently_playing_type")->valuestring);

        cJSON *device = cJSON_GetObjectItem(json, "device");
        if (device == NULL)
        {
            LOGE("No device object found in JSON");
            cJSON_Delete(json);
            http_client_.end();
            return playback_state;
        }
        else
        {
            strcpy(playback_state.device.id, cJSON_GetObjectItem(device, "id")->valuestring);
            playback_state.device.is_active = cJSON_GetObjectItem(device, "is_active")->valueint;
            playback_state.device.is_private_session = cJSON_GetObjectItem(device, "is_private_session")->valueint;
            playback_state.device.is_restricted = cJSON_GetObjectItem(device, "is_restricted")->valueint;
            strcpy(playback_state.device.name, cJSON_GetObjectItem(device, "name")->valuestring);
            strcpy(playback_state.device.type, cJSON_GetObjectItem(device, "type")->valuestring);
            playback_state.device.volume_percent = cJSON_GetObjectItem(device, "volume_percent")->valueint;
            playback_state.device.supports_volume = cJSON_GetObjectItem(device, "supports_volume")->valueint;
        }

        cJSON *item = cJSON_GetObjectItem(json, "item");
        if (item == NULL)
        {
            LOGE("No item object found in JSON");
            cJSON_Delete(json);
            http_client_.end();
            return playback_state;
        }
        else
        {
            cJSON *album = cJSON_GetObjectItem(item, "album");
            if (album == NULL)
            {
                LOGE("No album object found in JSON");
                cJSON_Delete(json);
                http_client_.end();
                return playback_state;
            }
            else
            {
                cJSON *images = cJSON_GetObjectItem(album, "images");
                if (images == NULL)
                {
                    LOGE("No images object found in JSON");
                    cJSON_Delete(json);
                    http_client_.end();
                    return playback_state; // add image not available bool
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
                            http_client_.end();
                            return playback_state; // add image not available bool
                        }
                        else
                        {
                            strcpy(playback_state.item.album.images[i].url, cJSON_GetObjectItem(image, "url")->valuestring);
                            playback_state.item.album.images[i].width = cJSON_GetObjectItem(image, "width")->valueint;
                            playback_state.item.album.images[i].height = cJSON_GetObjectItem(image, "height")->valueint;
                        }
                    }
                }

                strcpy(playback_state.item.album.name, cJSON_GetObjectItem(album, "name")->valuestring);
            }

            playback_state.item.duration_ms = cJSON_GetObjectItem(item, "duration_ms")->valueint;
            strcpy(playback_state.item.name, cJSON_GetObjectItem(item, "name")->valuestring);
            LOGE("Track name: %s", playback_state.item.name);
        }

        cJSON *actions = cJSON_GetObjectItem(json, "actions");
        if (actions == NULL)
        {
            LOGE("No actions object found in JSON");
            cJSON_Delete(json);
            http_client_.end();
            // return playback_state_;
        }
        else
        {
            // TODO optional field from api
            // playback_state.actions.interrupting_playback = cJSON_GetObjectItem(actions, "interrupting_playback")->valueint;
            // playback_state.actions.pausing = cJSON_GetObjectItem(actions, "pausing")->valueint;
            // playback_state.actions.resuming = cJSON_GetObjectItem(actions, "resuming")->valueint;
            // playback_state.actions.seeking = cJSON_GetObjectItem(actions, "seeking")->valueint;
            // playback_state.actions.skipping_next = cJSON_GetObjectItem(actions, "skipping_next")->valueint;
            // playback_state.actions.skipping_prev = cJSON_GetObjectItem(actions, "skipping_prev")->valueint;
            // playback_state.actions.toggling_repeat_context = cJSON_GetObjectItem(actions, "toggling_repeat_context")->valueint;
            // playback_state.actions.toggling_shuffle = cJSON_GetObjectItem(actions, "toggling_shuffle")->valueint;
            // playback_state.actions.toggling_repeat_track = cJSON_GetObjectItem(actions, "toggling_repeat_track")->valueint;
            // playback_state.actions.transferring_playback = cJSON_GetObjectItem(actions, "transferring_playback")->valueint;
        }

        cJSON_Delete(json); // Free memory
    }
    else if (httpCode == HTTP_CODE_NO_CONTENT)
    {
        LOGE("No content");
        playback_state.available = false;
        playback_state.spotify_available = true;
        return playback_state;
    }
    else
    {
        LOGE("Error in HTTP request: %d", httpCode);
        return playback_state;
    }
    http_client_.end();
    playback_state.available = true;
    playback_state.spotify_available = true;
    return playback_state;
}

bool SpotifyApi::refreshToken()
{
    http_client_.begin("https://accounts.spotify.com/api/token");
    http_client_.addHeader("Authorization", base64_basic_auth_header_);
    http_client_.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http_client_.addHeader("Content-Length", String(strlen(refresh_token_body_)));

    int httpCode = http_client_.POST(refresh_token_body_);

    if (httpCode == HTTP_CODE_OK)
    {
        // String payload = http_client_.getString();
        int content_length = http_client_.getSize();
        char *buffer_ = (char *)heap_caps_calloc(1, content_length, MALLOC_CAP_SPIRAM);
        if (buffer_ == nullptr)
        {
            LOGE("Failed to allocate memory for buffer");
            http_client_.end();
            return false;
        }

        WiFiClient *stream = http_client_.getStreamPtr();
        uint16_t total_bytes_read = 0;
        while (http_client_.connected() && (total_bytes_read < content_length))
        {
            // Read data in chunks and process
            size_t available_data = stream->available();
            if (available_data)
            {
                char buffer[128]; // Example buffer size
                uint16_t bytes_read = stream->readBytes(buffer, min(available_data, sizeof(buffer)));
                memcpy(buffer_ + total_bytes_read, buffer, bytes_read);
                assert(total_bytes_read + bytes_read <= content_length);
                total_bytes_read += bytes_read;
            }
        }

        cJSON *json = cJSON_Parse(buffer_);

        if (json == NULL)
        {
            LOGE("Error parsing JSON"); // TODO handle
            http_client_.end();
            return false;
        }

        PB_SpotifyConfig *spotify_config_ = (PB_SpotifyConfig *)heap_caps_calloc(1, sizeof(PB_SpotifyConfig), MALLOC_CAP_SPIRAM);

        strcpy(spotify_config_->access_token, cJSON_GetStringValue(cJSON_GetObjectItem(json, "access_token")));
        strcpy(spotify_config_->token_type, cJSON_GetStringValue(cJSON_GetObjectItem(json, "token_type")));
        spotify_config_->expires_in = cJSON_GetNumberValue(cJSON_GetObjectItem(json, "expires_in"));
        if (cJSON_HasObjectItem(json, "refresh_token"))
            strcpy(spotify_config_->refresh_token, cJSON_GetStringValue(cJSON_GetObjectItem(json, "refresh_token")));
        strcpy(spotify_config_->scope, cJSON_GetStringValue(cJSON_GetObjectItem(json, "scope")));
        cJSON_Delete(json);

        expires_in_ = spotify_config_->expires_in;

        configuration_.setSpotifyConfig(*spotify_config_);
        return true;
    }
    else
    {
        LOGE("Error in HTTP request: %d", httpCode);
    }
    http_client_.end();
    return false;
}

bool SpotifyApi::checkAndRefreshToken()
{
    if (last_refreshed_ms_ == 0 || millis() >= (last_refreshed_ms_ - 1000) + expires_in_ * 1000) // 10x for testing 3.6mins // seems to be working but mqtt and wifi having problems....
    {
        if (refreshToken())
        {
            last_refreshed_ms_ = millis();
            LOGE("Token refreshed");
            return true;
        }
        else
        {
            LOGE("Failed to refresh token");
            return false;
        }
    }
    return true;
}

bool SpotifyApi::play(const char *device_id)
{
    int16_t result = sendPutRequest("https://api.spotify.com/v1/me/player/play?device_id" + String(device_id));
    if (result == HTTP_CODE_OK || result == HTTP_CODE_NO_CONTENT)
    {
        return true;
    }
    LOGE("Error in HTTP request: %d", result);
    return false;
}

bool SpotifyApi::pause(const char *device_id)
{
    int result = sendPutRequest("https://api.spotify.com/v1/me/player/pause?device_id=" + String(device_id));
    if (result == HTTP_CODE_OK || result == HTTP_CODE_NO_CONTENT)
    {
        return true;
    }
    LOGE("Error in HTTP request: %d", result);
    return false;
}

int SpotifyApi::sendPutRequest(const String &url, const String &body)
{
    if (!checkAndRefreshToken())
    {
        LOGE("Failed to refresh token");
        return -1;
    }

    http_client_.begin(url);
    http_client_.addHeader("Authorization", auth_header_);
    http_client_.addHeader("Content-Type", "application/json");
    http_client_.addHeader("Content-Length", String(body.length()));

    int httpCode = http_client_.PUT(body);

    if (httpCode == HTTP_CODE_OK)
    {
        LOGE("Request successful");
    }
    else if (httpCode == HTTP_CODE_NO_CONTENT)
    {
        LOGE("No content"); // TODO tell user that the request was successful but no content was returned, ie no device is playing anything
    }
    else
    {
        LOGE("Error in HTTP request: %d", httpCode);
    }
    http_client_.end();
    return httpCode;
}

bool SpotifyApi::isAvailable() // TODO
{
    // if (spotify_config_.expires_in > 0)
    //     return checkAndRefreshToken();
    // return false;
    return true;
}

void SpotifyApi::downloadImage() // TODO
{
    // if (WiFi.status() == WL_CONNECTED)
    // {
    //     char imageUrl[128] = "";
    //     PlaybackState playback_state = getCurrentPlaybackState();
    //     if (!playback_state.available)
    //     {
    //         LOGW("Playback state not available");
    //         return;
    //     }

    //     for (int i = 0; i < MAX_IMAGES; i++)
    //     {
    //         if (playback_state.item.album.images[i].height == 300)
    //         {
    //             strcpy(imageUrl, playback_state.item.album.images[i].url);
    //             break;
    //         }
    //     }

    //     if (strlen(imageUrl) == 0)
    //     {
    //         LOGE("No image found");
    //         return;
    //     }

    //     LOGE("Downloading image from: %s", imageUrl);

    //     http_client_.setTimeout(1000);
    //     http_client_.begin(imageUrl);

    //     int httpCode = http_client_.GET();
    //     if (httpCode == HTTP_CODE_OK)
    //     {
    //         imageSize = http_client_.getSize();
    //         imageBuffer = (uint8_t *)heap_caps_aligned_alloc(16, imageSize, MALLOC_CAP_SPIRAM);

    //         if (imageBuffer != nullptr)
    //         {
    //             WiFiClient *stream = http_client_.getStreamPtr();
    //             size_t offset = 0;
    //             uint8_t buff[256];

    //             while (http_client_.connected() && (stream->available() > 0) && (offset < imageSize))
    //             {
    //                 int bytesRead = stream->readBytes(buff, sizeof(buff));
    //                 if (bytesRead > 0)
    //                 {
    //                     memcpy(imageBuffer + offset, buff, bytesRead);
    //                     offset += bytesRead;
    //                 }
    //             }

    //             LOGV(LOG_LEVEL_DEBUG, "Image downloaded and stored in PSRAM");
    //         }
    //         else
    //         {
    //             LOGE("Failed to allocate PSRAM for image.");
    //         }
    //     }
    //     else
    //     {
    //         LOGE("Error in HTTP request: %d", httpCode);
    //     }

    //     http_client_.end();
    // }
}

String SpotifyApi::getCurrentTrackName()
{
    return getCurrentPlaybackState().item.name;
}

void SpotifyApi::setConfig(const PB_SpotifyConfig &spotify_config)
{
    strncpy(auth_header_, "Bearer ", AUTH_HEADER_SIZE);
    strncat(auth_header_, spotify_config.access_token, AUTH_HEADER_SIZE - 1);
    auth_header_[AUTH_HEADER_SIZE] = '\0';

    strncpy(refresh_token_body_, "grant_type=refresh_token&refresh_token=", REFRESH_TOKEN_SIZE);
    strncat(refresh_token_body_, spotify_config.refresh_token, REFRESH_TOKEN_SIZE - 1);
    refresh_token_body_[BASE64_BASIC_AUTH_HEADER_SIZE] = '\0';

    strncpy(base64_basic_auth_header_, "Basic ", BASE64_BASIC_AUTH_HEADER_SIZE);
    strncat(base64_basic_auth_header_, spotify_config.base64_id_and_secret, BASE64_BASIC_AUTH_HEADER_SIZE - 1);
    base64_basic_auth_header_[BASE64_BASIC_AUTH_HEADER_SIZE] = '\0';

    expires_in_ = spotify_config.expires_in;
}