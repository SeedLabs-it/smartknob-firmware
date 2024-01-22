#include "../app.h"

class HassSetupApp : public App
{
public:
    HassSetupApp(TFT_eSprite *spr_);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);

    uint8_t navigationNext();
    uint8_t navigationBack();

private:
    void render_hass_setup_screen();
};