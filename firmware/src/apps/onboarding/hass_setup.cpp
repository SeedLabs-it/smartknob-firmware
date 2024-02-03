#include "hass_setup.h"

#include "font/NDS125_small.h"
#include "qrcode.h"

HassSetupApp::HassSetupApp(TFT_eSprite *spr_) : App(spr_), internal_state(HassSetupState::WIFI_CONNECT)
{
    back = MENU;
    motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        0,
        0,
        1,
        1,
        0.55, // Note the snap point is slightly past the midpoint (0.5); compare to normal detents which use a snap point *past* the next value (i.e. > 1)
        "SKDEMO_Light_switch",
        0,
        {},
        0,
        20,
    };
    startup_ms = millis();
}

EntityStateUpdate HassSetupApp::updateStateFromKnob(PB_SmartKnobState state)
{
    // needed to next reload of App
    motor_config.position_nonce = state.current_position;
    motor_config.position = state.current_position;

    return EntityStateUpdate{};
}

void HassSetupApp::updateStateFromSystem(AppState state) {}

int8_t HassSetupApp::navigationNext()
{
    switch (internal_state) // FOR NOW GO TO NEXT STATE ON BUTTON PRESS
    {
    case HassSetupState::WIFI_CONNECT:
        internal_state = HassSetupState::KNOB_URL;
        break;
    case HassSetupState::KNOB_URL:
        internal_state = HassSetupState::PROCESSING_WIFI;
        break;
    case HassSetupState::PROCESSING_WIFI:
        internal_state = HassSetupState::NQTT_CONFIG;
        break;
    case HassSetupState::NQTT_CONFIG:
        internal_state = HassSetupState::CONTINUE_HASS;
        break;
    case HassSetupState::CONTINUE_HASS:
        internal_state = HassSetupState::DONE;
        break;
    case HassSetupState::DONE:
        internal_state = HassSetupState::WIFI_CONNECT;
        return MENU; // ! SEND USER TO DEFAULT HASS APPS OR IF THEY HAVE ALREADY SET UP APPS IN HASS
        break;
    case HassSetupState::ERROR_WIFI:
        internal_state = HassSetupState::WIFI_CONNECT;
        break;
    case HassSetupState::ERROR_MQTT:
        internal_state = HassSetupState::NQTT_CONFIG;
        break;
    default:
        internal_state = HassSetupState::PROCESSING_WIFI;
        break;
    }

    return DONT_NAVIGATE;
}

int8_t HassSetupApp::navigationBack()
{
    internal_state = HassSetupState::WIFI_CONNECT;
    return MENU;
}

TFT_eSprite *HassSetupApp::render()
{
    switch (internal_state)
    {
    case HassSetupState::WIFI_CONNECT:
        return renderWifiConnect();
        break;
    case HassSetupState::KNOB_URL:
        return renderKnobUrl();
        break;
    case HassSetupState::PROCESSING_WIFI:
        return renderProcessing();
        break;
    case HassSetupState::NQTT_CONFIG:
        return renderMQTTConfig();
        break;
    case HassSetupState::CONTINUE_HASS:
        return renderContinueHass();
        break;
    case HassSetupState::DONE:
        return renderDone();
        break;
    case HassSetupState::ERROR_WIFI:
        return renderErrorWifi();
        break;
    case HassSetupState::ERROR_MQTT:
        return renderErrorMQTT();
        break;

    default:
        return renderProcessing();
        break;
    }
}

TFT_eSprite *HassSetupApp::renderWifiConnect()
{
    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_w = TFT_WIDTH / 2;

    int8_t screen_name_label_h = spr_->fontHeight(1);

    spr_->setTextDatum(CC_DATUM);
    spr_->setTextSize(1);
    spr_->setFreeFont(&NDS125_small);
    spr_->setTextColor(spr_->color565(128, 255, 80));

    spr_->drawString("SCAN TO CONNECT", center_w, screen_name_label_h * 3, 1);
    spr_->drawString("TO THE SMART KNOB", center_w, screen_name_label_h * 4, 1);

    QRCode qrcode;
    uint8_t qrcodeVersion = 6;
    uint8_t qrcodeData[qrcode_getBufferSize(qrcodeVersion)];
    std::string wifiqrcode = "WIFI:T:WPA;S:SMARTKNOB-AP;P:SMARTKNOB;H:;;https://seedlabs.it";
    qrcode_initText(&qrcode, qrcodeData, qrcodeVersion, 0, wifiqrcode.c_str());

    int moduleSize = 2;

    int qrCodeWidth = qrcode.size * moduleSize;
    int qrCodeHeight = qrcode.size * moduleSize;

    int startX = center_w - qrCodeWidth / 2;
    int startY = center_h - 4 - qrCodeHeight / 2;

    for (uint8_t y = 0; y < qrcode.size; y++)
    {
        for (uint8_t x = 0; x < qrcode.size; x++)
        {
            if (qrcode_getModule(&qrcode, x, y))
            {
                spr_->fillRect(startX + x * moduleSize, startY + y * moduleSize, moduleSize, moduleSize, TFT_WHITE);
            }
        }
    }

    spr_->drawString("OR CONNECT TO", center_w, TFT_WIDTH - screen_name_label_h * 4, 1);
    spr_->drawString("SMARTKNOB-AP WiFi", center_w, TFT_WIDTH - screen_name_label_h * 3, 1);

    return this->spr_;
}

TFT_eSprite *HassSetupApp::renderKnobUrl()
{
    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_w = TFT_WIDTH / 2;

    int8_t screen_name_label_h = spr_->fontHeight(1);

    spr_->setTextDatum(CC_DATUM);
    spr_->setTextSize(1);
    spr_->setFreeFont(&NDS125_small);
    spr_->setTextColor(spr_->color565(128, 255, 80));

    spr_->drawString("SCAN TO START", center_w, screen_name_label_h * 3, 1);
    spr_->drawString("SETUP", center_w, screen_name_label_h * 4, 1);

    QRCode qrcode;
    uint8_t qrcodeVersion = 6;
    uint8_t qrcodeData[qrcode_getBufferSize(qrcodeVersion)];
    std::string wifiqrcode = "https://seedlabs.it"; // ! LOCAL URL TO KNOB!!!
    qrcode_initText(&qrcode, qrcodeData, qrcodeVersion, 0, wifiqrcode.c_str());

    int moduleSize = 2;

    int qrCodeWidth = qrcode.size * moduleSize;
    int qrCodeHeight = qrcode.size * moduleSize;

    int startX = center_w - qrCodeWidth / 2;
    int startY = center_h - 4 - qrCodeHeight / 2;

    for (uint8_t y = 0; y < qrcode.size; y++)
    {
        for (uint8_t x = 0; x < qrcode.size; x++)
        {
            if (qrcode_getModule(&qrcode, x, y))
            {
                spr_->fillRect(startX + x * moduleSize, startY + y * moduleSize, moduleSize, moduleSize, TFT_WHITE);
            }
        }
    }

    spr_->drawString("OR OPEN 0.0.0.0", center_w, TFT_WIDTH - screen_name_label_h * 4, 1);
    spr_->drawString("IN YOUR BROWSER", center_w, TFT_WIDTH - screen_name_label_h * 3, 1);

    return this->spr_;
}

TFT_eSprite *HassSetupApp::renderProcessing()
{

    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_w = TFT_WIDTH / 2;

    long startup_diff_ms = millis() - startup_ms;
    uint32_t current_tick_time = startup_diff_ms / 24 % 120; // 120 ticks of 24ms each

    spr_->drawCircle(center_w, center_h, current_tick_time, TFT_WHITE);

    spr_->drawCircle(center_w, center_h, (current_tick_time + 60) % 120, TFT_WHITE); // concentric circle

    return this->spr_;
}

TFT_eSprite *HassSetupApp::renderMQTTConfig()
{
    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_w = TFT_WIDTH / 2;

    int8_t screen_name_label_h = spr_->fontHeight(1);

    spr_->setTextDatum(CC_DATUM);
    spr_->setTextSize(1);
    spr_->setFreeFont(&NDS125_small);
    spr_->setTextColor(spr_->color565(128, 255, 80));

    spr_->drawString("SCAN TO CONFIGURE", center_w, screen_name_label_h * 3, 1);
    spr_->drawString("MQTT", center_w, screen_name_label_h * 4, 1);

    QRCode qrcode;
    uint8_t qrcodeVersion = 6;
    uint8_t qrcodeData[qrcode_getBufferSize(qrcodeVersion)];
    std::string wifiqrcode = "https://seedlabs.it"; // ! LOCAL URL TO KNOB WHEN CONNECTED TO ROUTER/WIFI!!!
    qrcode_initText(&qrcode, qrcodeData, qrcodeVersion, 0, wifiqrcode.c_str());

    int moduleSize = 2;

    int qrCodeWidth = qrcode.size * moduleSize;
    int qrCodeHeight = qrcode.size * moduleSize;

    int startX = center_w - qrCodeWidth / 2;
    int startY = center_h - 4 - qrCodeHeight / 2;

    for (uint8_t y = 0; y < qrcode.size; y++)
    {
        for (uint8_t x = 0; x < qrcode.size; x++)
        {
            if (qrcode_getModule(&qrcode, x, y))
            {
                spr_->fillRect(startX + x * moduleSize, startY + y * moduleSize, moduleSize, moduleSize, TFT_WHITE);
            }
        }
    }

    spr_->drawString("OR OPEN 0.0.0.0", center_w, TFT_WIDTH - screen_name_label_h * 4, 1);
    spr_->drawString("IN YOUR BROWSER", center_w, TFT_WIDTH - screen_name_label_h * 3, 1);

    return this->spr_;
}

TFT_eSprite *HassSetupApp::renderContinueHass()
{
    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_w = TFT_WIDTH / 2;

    int8_t screen_name_label_h = spr_->fontHeight(1);

    spr_->setTextDatum(CC_DATUM);
    spr_->setTextSize(2);
    spr_->setFreeFont(&NDS125_small);
    spr_->setTextColor(TFT_SKYBLUE);

    spr_->drawString("CONTINUE THE SETUP IN", center_w, center_h - screen_name_label_h / 1.8, 1);
    spr_->drawString("HOME ASSISTANT", center_w, center_h + screen_name_label_h / 1.8, 1);

    return this->spr_;
}

TFT_eSprite *HassSetupApp::renderDone()
{
    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_w = TFT_WIDTH / 2;

    int8_t screen_name_label_h = spr_->fontHeight(1);

    spr_->setTextDatum(CC_DATUM);
    spr_->setTextSize(2);
    spr_->setFreeFont(&NDS125_small);

    spr_->setTextColor(TFT_WHITE);
    spr_->drawString("YOUR ALL SET", center_w, screen_name_label_h + 12, 1);

    spr_->drawBitmap(center_w - 96 / 2, center_h - 96 / 1.65, seedlabs_logo, 96, 96, TFT_WHITE, TFT_BLACK);

    spr_->setTextColor(spr_->color565(128, 255, 80));
    spr_->drawString("PRESS TO CONTINUE", center_w, TFT_HEIGHT - screen_name_label_h * 2 - 12, 1);

    return this->spr_;
}

TFT_eSprite *HassSetupApp::renderErrorWifi()
{
    return this->spr_;
}

TFT_eSprite *HassSetupApp::renderErrorMQTT()
{
    return this->spr_;
}