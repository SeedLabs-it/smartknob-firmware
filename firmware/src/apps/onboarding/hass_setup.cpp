#include "hass_setup.h"

#include "font/NDS125_small.h"
#include "qrcode.h"

HassSetupApp::HassSetupApp(TFT_eSprite *spr_) : App(spr_)
{
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
}

EntityStateUpdate HassSetupApp::updateStateFromKnob(PB_SmartKnobState state)
{
    // needed to next reload of App
    motor_config.position_nonce = state.current_position;
    motor_config.position = state.current_position;

    return EntityStateUpdate{};
}

void HassSetupApp::updateStateFromSystem(AppState state) {}

TFT_eSprite *HassSetupApp::render()
{
    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_w = TFT_WIDTH / 2;

    int8_t screen_name_label_h = spr_->fontHeight(1);

    spr_->setTextDatum(CC_DATUM);
    spr_->setTextSize(1);
    spr_->setFreeFont(&NDS125_small);

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
