
#include "spotify_api.h"

//

bool SpotifyApi::isPlaying()
{
    checkAndRefreshToken();
    delay(50);
    return getCurrentPlaybackState().is_playing;
}

PlaybackState SpotifyApi::getCurrentPlaybackState()
{
    checkAndRefreshToken();

    PlaybackState playback_state_;
    playback_state_.available = false;

    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Bearer %s", spotify_config_.access_token);

    HTTPClient http;
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
            return playback_state_;
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
            return playback_state_;
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
            return playback_state_;
        }
        else
        {
            cJSON *album = cJSON_GetObjectItem(item, "album");
            if (album == NULL)
            {
                LOGE("No album object found in JSON");
                cJSON_Delete(json);
                http.end();
                return playback_state_;
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
        }

        cJSON *actions = cJSON_GetObjectItem(json, "actions");
        if (actions == NULL)
        {
            LOGE("No actions object found in JSON");
            cJSON_Delete(json);
            http.end();
            return playback_state_;
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

        playback_state_.available = true;
        return playback_state_;
    }
    else
    {
        LOGE("Error in HTTP request: %d", httpCode);
        return playback_state_;
    }
    http.end();
    playback_state_.available = true;
    return playback_state_;
}

void SpotifyApi::refreshToken()
{
    HTTPClient http;
    http.begin("https://accounts.spotify.com/api/token");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Basic %s", spotify_config_.base64_id_and_secret);
    http.addHeader("Authorization", auth_header);

    char body[512];
    snprintf(body, sizeof(body), "grant_type=refresh_token&refresh_token=%s", spotify_config_.refresh_token);
    http.addHeader("Content-Length", String(strlen(body)));

    int httpCode = http.POST(body);

    if (httpCode == HTTP_CODE_OK)
    {
        String payload = http.getString();
        // TODO store the new token in the config
    }
    else
    {
        LOGE("Error in HTTP request: %d", httpCode);
    }
    http.end();
}

bool SpotifyApi::checkAndRefreshToken()
{
    if (millis() >= last_refreshed_ + spotify_config_.expires_in * 1000) // 10x for testing 3.6mins
    {
        // LOGE("Token expired, refreshing");
        // return refreshToken()
        // refreshToken(); //TODO disable until we store the new token
        last_refreshed_ = millis(); // 10x for testing 3.6mins
        LOGE("Token refreshed");
    }
    // return getCurrentPlaybackState();
    return true;
}

bool SpotifyApi::play()
{
    return sendPutRequest("https://api.spotify.com/v1/me/player/play");
}

bool SpotifyApi::pause()
{
    return sendPutRequest("https://api.spotify.com/v1/me/player/pause");
}

bool SpotifyApi::sendPutRequest(const String &url, const String &body)

{
    checkAndRefreshToken();

    char auth_header[512];
    snprintf(auth_header, sizeof(auth_header), "Bearer %s", spotify_config_.access_token);

    HTTPClient http;
    http.begin(url);
    http.addHeader("Authorization", auth_header);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Content-Length", String(body.length()));

    int httpCode = http.PUT(body);

    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_NO_CONTENT)
    {
        LOGE("Request successful");
        return true;
    }
    else
    {
        LOGE("Error in HTTP request: %d", httpCode);
    }
    http.end();
    return false;
}

void SpotifyApi::setConfig(PB_SpotifyConfig spotify_config)
{
    spotify_config_ = spotify_config;
}

bool SpotifyApi::isAvailable()
{
    if (spotify_config_.expires_in > 0)
        return checkAndRefreshToken();
    return false;
}