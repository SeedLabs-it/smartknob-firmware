#include "onboarding_flow.h"

OnboardingFlow::OnboardingFlow(TFT_eSprite *spr_, std::string entity_name) : App(spr_)
{

    this->entity_name = entity_name;

    motor_config = PB_SmartKnobConfig{
        0,
        0,
        0,
        0,
        20,
        230.0 / 20.0 * PI / 180,
        1,
        1,
        1.1,
        "SKDEMO_Music", // TODO: clean this
        0,
        {},
        0,
        90,
    };
}

EntityStateUpdate OnboardingFlow::updateStateFromKnob(PB_SmartKnobState state)
{
    current_volume_position = state.current_position;
    current_volume = current_volume_position * 5;

    // needed to next reload of App
    motor_config.position_nonce = current_volume_position;
    motor_config.position = current_volume_position;

    EntityStateUpdate new_state;

    // new_state.entity_name = entity_name;
    // new_state.new_value = current_volume * 1.0;

    if (last_volume != current_volume)
    {
        last_volume = current_volume;
        new_state.changed = true;
        sprintf(new_state.app_slug, "%s", APP_SLUG_MUSIC);
    }

    return new_state;
}

void OnboardingFlow::updateStateFromSystem(AppState state) {}

TFT_eSprite *OnboardingFlow::render()
{

    return this->spr_;
};