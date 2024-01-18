#include "../app.h"

#include "../apps.h"

class DemoApp : public App
{
public:
    DemoApp(TFT_eSprite *spr_);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);
    uint8_t navigationNext();
    std::string getClassName();

private:
    Apps *apps;
    uint8_t demo_apps_count = 0;
    uint8_t current_demo_position = 0;
    SemaphoreHandle_t mutex_;

    App find(uint8_t id);
    void render_demo_screen();
};