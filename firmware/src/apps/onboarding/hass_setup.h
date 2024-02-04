#include "../app.h"

enum class HassSetupState
{
    WIFI_CONNECT,
    KNOB_URL,
    PROCESSING_WIFI,
    NQTT_CONFIG,
    CONTINUE_HASS,
    DONE,
    ERROR_WIFI,
    ERROR_MQTT
};

class HassSetupApp : public App
{
public:
    HassSetupApp(TFT_eSprite *spr_);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);

    // int8_t navigationNext();
    int8_t navigationBack();

protected:
private:
    HassSetupState internal_state;
    AppState state;

    TFT_eSprite *renderWifiConnect();
    TFT_eSprite *renderKnobUrl();
    TFT_eSprite *renderProcessing();
    TFT_eSprite *renderMQTTConfig();
    TFT_eSprite *renderContinueHass();
    TFT_eSprite *renderDone();
    TFT_eSprite *renderErrorWifi();
    TFT_eSprite *renderErrorMQTT();

    unsigned long startup_ms;
};