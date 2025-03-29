
#include "spotify_api.h"

AsyncClient *cover_art_client = nullptr;
AsyncClient *cover_art_colors_client = nullptr;

AsyncHttp *cover_art_http = nullptr;
AsyncHttp *cover_art_colors_http = nullptr;

#define HOST "configurator.seedlabs.it"
#define PORT 80

SpotifyApi::SpotifyApi(Configuration &configuration)
    : configuration_(configuration)
{
    auth_header_ =
        (char *)heap_caps_calloc(1, AUTH_HEADER_SIZE, MALLOC_CAP_SPIRAM);
    assert(auth_header_ != nullptr);

    refresh_token_body_ =
        (char *)heap_caps_calloc(1, REFRESH_TOKEN_SIZE, MALLOC_CAP_SPIRAM);
    assert(refresh_token_body_ != nullptr);

    base64_basic_auth_header_ = (char *)heap_caps_calloc(
        1, BASE64_BASIC_AUTH_HEADER_SIZE, MALLOC_CAP_SPIRAM);
    assert(base64_basic_auth_header_ != nullptr);

    cover_art_client = new AsyncClient();
    cover_art_colors_client = new AsyncClient();

    cover_art_http = new AsyncHttp(cover_art_client, HOST, PORT);
    cover_art_colors_http = new AsyncHttp(cover_art_colors_client, HOST, PORT);
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

void processImage(char *body, size_t length)
{
    LOGD("Processing image of size: %zu bytes", length);

    // Example: Save to a file or decode the image
    // Here you can decode the image (e.g., using a JPEG/PNG decoder)
}

PlaybackState SpotifyApi::getCurrentPlaybackState()
{
    PlaybackState playback_state{.available = false,
                                 .spotify_available = false,
                                 .timestamp = 0,
                                 .progress_ms = 0};
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
        char *buffer_ =
            (char *)heap_caps_calloc(1, content_length, MALLOC_CAP_SPIRAM);
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
                uint16_t bytes_read =
                    stream->readBytes(buffer, min(available_data, sizeof(buffer)));
                memcpy(buffer_ + total_bytes_read, buffer, bytes_read);

                assert(total_bytes_read + bytes_read <= content_length);

                total_bytes_read += bytes_read;
            }
        }

        cJSON *json = cJSON_Parse(buffer_);

        heap_caps_free(buffer_);

        if (json == NULL)
        {
            LOGE("Error parsing JSON");
            http_client_.end();
            playback_state.available = false;
            return playback_state;
        }

        strcpy(playback_state.repeat_state,
               cJSON_GetObjectItem(json, "repeat_state")->valuestring);
        playback_state.shuffle_state =
            cJSON_GetObjectItem(json, "shuffle_state")->valueint;
        playback_state.timestamp =
            cJSON_GetObjectItem(json, "timestamp")->valuedouble;
        playback_state.progress_ms =
            cJSON_GetObjectItem(json, "progress_ms")->valueint;
        playback_state.is_playing =
            cJSON_GetObjectItem(json, "is_playing")->valueint;
        strcpy(playback_state.currently_playing_type,
               cJSON_GetObjectItem(json, "currently_playing_type")->valuestring);

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
            strcpy(playback_state.device.id,
                   cJSON_GetObjectItem(device, "id")->valuestring);
            playback_state.device.is_active =
                cJSON_GetObjectItem(device, "is_active")->valueint;
            playback_state.device.is_private_session =
                cJSON_GetObjectItem(device, "is_private_session")->valueint;
            playback_state.device.is_restricted =
                cJSON_GetObjectItem(device, "is_restricted")->valueint;
            strcpy(playback_state.device.name,
                   cJSON_GetObjectItem(device, "name")->valuestring);
            strcpy(playback_state.device.type,
                   cJSON_GetObjectItem(device, "type")->valuestring);
            playback_state.device.volume_percent =
                cJSON_GetObjectItem(device, "volume_percent")->valueint;
            playback_state.device.supports_volume =
                cJSON_GetObjectItem(device, "supports_volume")->valueint;
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
            cJSON *artists = cJSON_GetObjectItem(item, "artists");

            if (artists == NULL)
            {
                LOGE("No artists object found in JSON");
                cJSON_Delete(json);
                http_client_.end();
                return playback_state;
            }
            else
            {
                strcpy(playback_state.item.artist,
                       cJSON_GetObjectItem(cJSON_GetArrayItem(artists, 0), "name")
                           ->valuestring); // only first artist for now
            }

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
                            strcpy(playback_state.item.album.images[i].url,
                                   cJSON_GetObjectItem(image, "url")->valuestring);
                            playback_state.item.album.images[i].width =
                                cJSON_GetObjectItem(image, "width")->valueint;
                            playback_state.item.album.images[i].height =
                                cJSON_GetObjectItem(image, "height")->valueint;
                        }
                    }
                }

                strcpy(playback_state.item.album.name,
                       cJSON_GetObjectItem(album, "name")->valuestring);
            }

            playback_state.item.duration_ms =
                cJSON_GetObjectItem(item, "duration_ms")->valueint;
            strcpy(playback_state.item.name,
                   cJSON_GetObjectItem(item, "name")->valuestring);
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
            // playback_state.actions.interrupting_playback =
            // cJSON_GetObjectItem(actions, "interrupting_playback")->valueint;
            // playback_state.actions.pausing = cJSON_GetObjectItem(actions,
            // "pausing")->valueint; playback_state.actions.resuming =
            // cJSON_GetObjectItem(actions, "resuming")->valueint;
            // playback_state.actions.seeking = cJSON_GetObjectItem(actions,
            // "seeking")->valueint; playback_state.actions.skipping_next =
            // cJSON_GetObjectItem(actions, "skipping_next")->valueint;
            // playback_state.actions.skipping_prev = cJSON_GetObjectItem(actions,
            // "skipping_prev")->valueint;
            // playback_state.actions.toggling_repeat_context =
            // cJSON_GetObjectItem(actions, "toggling_repeat_context")->valueint;
            // playback_state.actions.toggling_shuffle = cJSON_GetObjectItem(actions,
            // "toggling_shuffle")->valueint;
            // playback_state.actions.toggling_repeat_track =
            // cJSON_GetObjectItem(actions, "toggling_repeat_track")->valueint;
            // playback_state.actions.transferring_playback =
            // cJSON_GetObjectItem(actions, "transferring_playback")->valueint;
        }

        cJSON_Delete(json); // Free memory
    }
    else if (httpCode == HTTP_CODE_NO_CONTENT)
    {
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

    LOGE("Refresh token body: %s", refresh_token_body_);
    LOGE("Refresh token header: %s", base64_basic_auth_header_);

    if (httpCode == HTTP_CODE_OK)
    {
        // String payload = http_client_.getString();
        int content_length = http_client_.getSize();
        char *buffer_ =
            (char *)heap_caps_calloc(1, content_length, MALLOC_CAP_SPIRAM);
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
                uint16_t bytes_read =
                    stream->readBytes(buffer, min(available_data, sizeof(buffer)));
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

        PB_SpotifyConfig *spotify_config_ = (PB_SpotifyConfig *)heap_caps_calloc(
            1, sizeof(PB_SpotifyConfig), MALLOC_CAP_SPIRAM);

        strcpy(spotify_config_->access_token,
               cJSON_GetStringValue(cJSON_GetObjectItem(json, "access_token")));
        strcpy(spotify_config_->token_type,
               cJSON_GetStringValue(cJSON_GetObjectItem(json, "token_type")));
        spotify_config_->expires_in =
            cJSON_GetNumberValue(cJSON_GetObjectItem(json, "expires_in")) * 1000;
        if (cJSON_HasObjectItem(json, "refresh_token"))
            strcpy(spotify_config_->refresh_token,
                   cJSON_GetStringValue(cJSON_GetObjectItem(json, "refresh_token")));
        strcpy(spotify_config_->scope,
               cJSON_GetStringValue(cJSON_GetObjectItem(json, "scope")));
        cJSON_Delete(json);

        expires_in_ = spotify_config_->expires_in;

        configuration_.setSpotifyConfig(*spotify_config_);
        http_client_.end();
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
    // LOGE("Last refreshed: %lu", last_refreshed_ms_);
    // LOGE("Expires in: %d", expires_in_);
    if (last_refreshed_ms_ == 0 || millis() >= (last_refreshed_ms_ - 1000) + 30000)
    {
        if (refreshToken())
        {
            last_refreshed_ms_ = millis();
            LOGV(LOG_LEVEL_INFO, "Spotify token refreshed");
            return true;
        }
        else
        {
            LOGE("Failed to refresh Spotify token");
            return false;
        }
    }
    return true;
}

bool SpotifyApi::play(const char *device_id)
{
    int16_t result = -255;
    if (strcmp(device_id, "") == 0)
    {
        result =
            sendPutRequest("https://api.spotify.com/v1/me/player/play?device_id=" +
                           String(device_id_));
    }
    else
    {
        result =
            sendPutRequest("https://api.spotify.com/v1/me/player/play?device_id=" +
                           String(device_id));
    }
    if (result == HTTP_CODE_OK || result == HTTP_CODE_NO_CONTENT)
    {
        return true;
    }
    LOGE("Error in HTTP request: %d", result);
    return false;
}

bool SpotifyApi::pause(const char *device_id)
{
    int16_t result = -255;
    if (strcmp(device_id, "") == 0)
    {
        result =
            sendPutRequest("https://api.spotify.com/v1/me/player/pause?device_id=" +
                           String(device_id_));
    }
    else
    {
        result =
            sendPutRequest("https://api.spotify.com/v1/me/player/pause?device_id=" +
                           String(device_id));
    }
    if (result == HTTP_CODE_OK || result == HTTP_CODE_NO_CONTENT)
    {
        return true;
    }
    LOGE("Error in HTTP request: %d", result);
    return false;
}

bool SpotifyApi::setVolume(uint8_t volume, const char *device_id)
{
    int result = sendPutRequest(
        "https://api.spotify.com/v1/me/player/volume?volume_percent=" +
        String(volume) + "&device_id=" + String(device_id));
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
        return -1;
    }

    http_client_.begin(url);
    http_client_.addHeader("Authorization", auth_header_);
    http_client_.addHeader("Content-Type", "application/json");
    http_client_.addHeader("Content-Length", String(body.length()));

    int httpCode = http_client_.PUT(body);

    if (httpCode == HTTP_CODE_OK)
    {
        LOGV(LOG_LEVEL_DEBUG, "HTTP_CODE_OK");
    }
    else if (httpCode == HTTP_CODE_NO_CONTENT)
    {
        LOGV(LOG_LEVEL_DEBUG, "HTTP_CODE_NO_CONTENT");
    }
    else
    {
        LOGE("Error in HTTP request code: %d", httpCode);
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

void SpotifyApi::downloadImage(char *path) // TODO
{
    if (WiFi.status() == WL_CONNECTED)
    {
        cover_art_http->get(path, "image/png", [this](const char *body, size_t body_size)
                            {
                imageSize = body_size;
                imageBuffer = (uint8_t *)heap_caps_aligned_alloc(16, body_size, MALLOC_CAP_SPIRAM);
                memcpy(imageBuffer, body, body_size);

                lv_img_dsc_t *image_dsc = (lv_img_dsc_t *)heap_caps_malloc(sizeof(lv_img_dsc_t), MALLOC_CAP_SPIRAM);
                image_dsc->header.always_zero = 0;
                image_dsc->header.w = 240;
                image_dsc->header.h = 240;
                image_dsc->header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
                image_dsc->data = imageBuffer;
                image_dsc->data_size = body_size;

                WiFiEvent cover_art_event;
                cover_art_event.type = SK_SPOTIFY_NEW_COVER_ART;
                cover_art_event.body.cover_art = image_dsc;
                publishEvent(cover_art_event);
                
                heap_caps_free(imageBuffer); });
    }
}

void SpotifyApi::fetchImageColors(char *path)
{
    if (WiFi.status() == WL_CONNECTED)
    {
        cover_art_colors_http->get(path, "application/json", [this](const char *body, size_t body_size)
                                   {
            cJSON *json = cJSON_Parse(body);
            if (json == NULL)
            {
                LOGE("Error parsing JSON");
                return;
            }

            cJSON *color1 = cJSON_GetObjectItem(json, "color1"); 
            cJSON *color2 = cJSON_GetObjectItem(json, "color2");

            if (color1 == NULL || color2 == NULL)
            {
                LOGE("Error parsing JSON");
                return;
            }

            colors = (lv_color_t *)heap_caps_malloc(2 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
            colors[0] = lv_color_hex(color1->valueint);
            colors[1] = lv_color_hex(color2->valueint);

            WiFiEvent cover_art_colors_event;
            cover_art_colors_event.type = SK_SPOTIFY_NEW_COVER_ART_COLORS;
            cover_art_colors_event.body.cover_art_colors = colors;
            publishEvent(cover_art_colors_event); });
    }
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

    strncpy(refresh_token_body_,
            "grant_type=refresh_token&refresh_token=", REFRESH_TOKEN_SIZE);
    strncat(refresh_token_body_, spotify_config.refresh_token,
            REFRESH_TOKEN_SIZE - 1);
    refresh_token_body_[BASE64_BASIC_AUTH_HEADER_SIZE] = '\0';

    strncpy(base64_basic_auth_header_, "Basic ", BASE64_BASIC_AUTH_HEADER_SIZE);
    strncat(base64_basic_auth_header_, spotify_config.base64_id_and_secret,
            BASE64_BASIC_AUTH_HEADER_SIZE - 1);
    base64_basic_auth_header_[BASE64_BASIC_AUTH_HEADER_SIZE] = '\0';

    expires_in_ = spotify_config.expires_in;
    strcpy(device_id_, spotify_config.device_id);
}

void SpotifyApi::setSharedEventsQueue(QueueHandle_t shared_events_queue)
{
    shared_events_queue_ = shared_events_queue;
}

void SpotifyApi::publishEvent(WiFiEvent event)
{
    if (shared_events_queue_ == NULL)
    {
        LOGE("No shared events queue set");
        return; // TODO handle!!!!!!!!!
    }
    event.sent_at = millis();
    xQueueSendToBack(shared_events_queue_, &event, 0);
}
