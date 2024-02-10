#include "onboarding_flow.h"

OnboardingFlow::OnboardingFlow()
{
    // just for initialisation of the firmware
}

OnboardingFlow::OnboardingFlow(TFT_eSprite *spr_)
{
    this->spr_ = spr_;

    motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        5,
        90 * PI / 180,
        1,
        1,
        1.1,
        "",
        0,
        {},
        0,
        90,
    };
}

EntityStateUpdate OnboardingFlow::update(AppState state)
{
    updateStateFromSystem(state);
    return updateStateFromKnob(state.motor_state);
}

EntityStateUpdate OnboardingFlow::updateStateFromKnob(PB_SmartKnobState state)
{
    current_position = state.current_position;

    // needed to next reload of App
    motor_config.position_nonce = current_position;
    motor_config.position = current_position;

    EntityStateUpdate new_state;

    return new_state;
}

void OnboardingFlow::updateStateFromSystem(AppState state) {}

TFT_eSprite *OnboardingFlow::renderWelcomeScreen()
{
    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;

    spr_->setTextDatum(CC_DATUM);

    sprintf(buf_, "SMART KNOB");
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(default_text_color);
    spr_->drawString(buf_, center_h, 70, 1);

    // TODO move this to a constant of software/hardware
    sprintf(buf_, "DEV KIT V%d.%d", 0, 1);
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(default_text_color);
    spr_->drawString(buf_, center_h, 100, 1);

    sprintf(buf_, "ROTATE TO START");
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(accent_text_color);
    spr_->drawString(buf_, center_h, 170, 1);

    return this->spr_;
}
TFT_eSprite *OnboardingFlow::renderHass1StepScreen()
{
    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;

    spr_->setTextDatum(CC_DATUM);

    sprintf(buf_, "HOME ASSISTANT");
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(default_text_color);
    spr_->drawString(buf_, center_h, 50, 1);

    sprintf(buf_, "INTEGRATION");
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(default_text_color);
    spr_->drawString(buf_, center_h, 80, 1);

    sprintf(buf_, "PRESS TO CONFIGURE");
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(accent_text_color);
    spr_->drawString(buf_, center_h, 190, 1);

    return this->spr_;
}
TFT_eSprite *OnboardingFlow::renderHass2StepScreen() {}
TFT_eSprite *OnboardingFlow::renderHass3StepScreen() {}
TFT_eSprite *OnboardingFlow::renderHass4StepScreen() {}
TFT_eSprite *OnboardingFlow::renderHass5StepScreen() {}
TFT_eSprite *OnboardingFlow::renderHass6StepScreen() {}
TFT_eSprite *OnboardingFlow::renderHass7StepScreen() {}
TFT_eSprite *OnboardingFlow::renderWiFi1StepScreen() {}
TFT_eSprite *OnboardingFlow::renderDemo1StepScreen() {}
TFT_eSprite *OnboardingFlow::renderAboutScreen() {}

TFT_eSprite *OnboardingFlow::render()
{

    // ESP_LOGD("onboarding", "%d", current_position);

    switch (current_position)
    {
    case 0:
        return renderWelcomeScreen();
    case 1:
        return renderHass1StepScreen();

    default:
        break;
    }

    return this->spr_;
};