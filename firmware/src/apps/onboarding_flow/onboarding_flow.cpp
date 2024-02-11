#include "onboarding_flow.h"

OnboardingFlow::OnboardingFlow()
{

    root_level_motor_config = PB_SmartKnobConfig{
        1,
        0,
        1,
        0,
        4,
        35 * PI / 180,
        2,
        1,
        0.55,
        "",
        0,
        {},
        0,
        20,
    };

    blocked_motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        0,
        55 * PI / 180,
        0.01,
        0.6,
        1.1,
        "",
        0,
        {},
        0,
        90,
    };
}

OnboardingFlow::OnboardingFlow(TFT_eSprite *spr_)
{
    this->spr_ = spr_;

    root_level_motor_config = PB_SmartKnobConfig{
        1,
        0,
        1,
        0,
        4,
        35 * PI / 180,
        2,
        1,
        0.55,
        "",
        0,
        {},
        0,
        20,
    };

    blocked_motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        0,
        55 * PI / 180,
        0.01,
        0.6,
        1.1,
        "",
        0,
        {},
        0,
        90,
    };
}

void OnboardingFlow::triggerMotorConfigUpdate()
{
    if (this->motor_updater != nullptr)
    {
        motor_updater->requestUpdate(root_level_motor_config);
    }
}

void OnboardingFlow::setMotorUpdater(MotorUpdater *motor_updater)
{
    this->motor_updater = motor_updater;
}

EntityStateUpdate OnboardingFlow::update(AppState state)
{
    updateStateFromSystem(state);
    return updateStateFromKnob(state.motor_state);
}

void OnboardingFlow::handleNavigationEvent(NavigationEvent event)
{

    if (event.press == NAVIGATION_EVENT_PRESS_SHORT)
    {
        switch (current_page)
        {
        case ONBOARDING_FLOW_PAGE_STEP_HASS_1:
            current_page = ONBOARDING_FLOW_PAGE_STEP_HASS_2;

            motor_updater->requestUpdate(blocked_motor_config);
            break;

        default:
            break;
        }
    }

    if (event.press == NAVIGATION_EVENT_PRESS_LONG)
    {
        switch (current_page)
        {
        case ONBOARDING_FLOW_PAGE_STEP_HASS_2:
            current_page = ONBOARDING_FLOW_PAGE_STEP_HASS_1;

            motor_updater->requestUpdate(root_level_motor_config);

            break;

        default:
            break;
        }
    }
}

EntityStateUpdate OnboardingFlow::updateStateFromKnob(PB_SmartKnobState state)
{
    current_position = state.current_position;

    // this works only at the top menu
    if (current_page < 5)
    {
        current_page = current_position;

        // needed to next reload of App
        root_level_motor_config.position_nonce = current_position;
        root_level_motor_config.position = current_position;
    }

    EntityStateUpdate new_state;

    return new_state;
}

void OnboardingFlow::updateStateFromSystem(AppState state) {}

TFT_eSprite *OnboardingFlow::renderWelcomePage()
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
    spr_->drawString(buf_, center_h, 185, 1);

    return this->spr_;
}
TFT_eSprite *OnboardingFlow::renderHass1StepPage()
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
    spr_->drawString(buf_, center_h, 185, 1);

    return this->spr_;
}
TFT_eSprite *OnboardingFlow::renderHass2StepPage()
{
    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;

    spr_->setTextDatum(CC_DATUM);

    sprintf(buf_, "SCAN TO CONNECT");
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(accent_text_color);
    spr_->drawString(buf_, center_h, 50, 1);

    sprintf(buf_, "THE SMARTKNOB");
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(accent_text_color);
    spr_->drawString(buf_, center_h, 80, 1);

    sprintf(buf_, "OR CONNECT TO");
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(accent_text_color);
    spr_->drawString(buf_, center_h, 155, 1);

    sprintf(buf_, "SMARTKNOB-AP WIFI");
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(accent_text_color);
    spr_->drawString(buf_, center_h, 185, 1);

    return this->spr_;
}
TFT_eSprite *OnboardingFlow::renderHass3StepPage()
{
    return this->spr_;
}
TFT_eSprite *OnboardingFlow::renderHass4StepPage()
{
    return this->spr_;
}
TFT_eSprite *OnboardingFlow::renderHass5StepPage()
{
    return this->spr_;
}
TFT_eSprite *OnboardingFlow::renderHass6StepPage()
{
    return this->spr_;
}
TFT_eSprite *OnboardingFlow::renderHass7StepPage()
{
    return this->spr_;
}
TFT_eSprite *OnboardingFlow::renderWiFi1StepPage()
{
    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;

    spr_->setTextDatum(CC_DATUM);

    sprintf(buf_, "WIFI");
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(default_text_color);
    spr_->drawString(buf_, center_h, 50, 1);

    sprintf(buf_, "PRESS TO CONFIGURE");
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(accent_text_color);
    spr_->drawString(buf_, center_h, 185, 1);

    return this->spr_;
}
TFT_eSprite *OnboardingFlow::renderDemo1StepPage()
{
    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;

    spr_->setTextDatum(CC_DATUM);

    sprintf(buf_, "DEMO MODE");
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(default_text_color);
    spr_->drawString(buf_, center_h, 50, 1);

    sprintf(buf_, "PRESS TO START");
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(accent_text_color);
    spr_->drawString(buf_, center_h, 185, 1);

    return this->spr_;
}
TFT_eSprite *OnboardingFlow::renderAboutPage()
{
    uint16_t center_h = TFT_WIDTH / 2;
    uint16_t center_v = TFT_WIDTH / 2;

    spr_->setTextDatum(CL_DATUM);

    uint8_t left_padding = 30;

    sprintf(buf_, "FIRMWARE 0.1b");
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(default_text_color);
    spr_->drawString(buf_, left_padding, 50, 1);

    sprintf(buf_, "HARDWARE: %s", "DEVKIT V0.1");
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(default_text_color);
    spr_->drawString(buf_, left_padding, 80, 1);

    sprintf(buf_, "WIFI: %s", "NOT CONNECTED");
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(default_text_color);
    spr_->drawString(buf_, left_padding, 110, 1);

    sprintf(buf_, "SEEDLABS.IT®");
    spr_->setFreeFont(&NDS1210pt7b);
    spr_->setTextColor(accent_text_color);
    spr_->drawString(buf_, left_padding, 185, 1);

    return this->spr_;
}

TFT_eSprite *OnboardingFlow::render()
{
    switch (current_page)
    {
    case ONBOARDING_FLOW_PAGE_STEP_WELCOME:
        return renderWelcomePage();
    case ONBOARDING_FLOW_PAGE_STEP_HASS_1:
        return renderHass1StepPage();
    case ONBOARDING_FLOW_PAGE_STEP_HASS_2:
        return renderHass2StepPage();
    case ONBOARDING_FLOW_PAGE_STEP_WIFI_1:
        return renderWiFi1StepPage();
    case ONBOARDING_FLOW_PAGE_STEP_DEMO_1:
        return renderDemo1StepPage();
    case ONBOARDING_FLOW_PAGE_STEP_ABOUT:
        return renderAboutPage();

    default:
        break;
    }

    return this->spr_;
};