#include "../app.h"

#include "../../font/NDS1210pt7b.h"
#include "../../font/Pixel62mr11pt7b.h"

class PrinterChamberApp : public App
{
public:
    PrinterChamberApp(TFT_eSprite *spr_, char *entity_name);
    TFT_eSprite *render();
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);
    void updateStateFromSystem(AppState state);

private:
    uint8_t current_temperature = 0;
    uint8_t wanted_temperature = 0;
    uint8_t last_wanted_temperature = 0;
    uint32_t wanted_temperature_position = 0;
    uint8_t num_positions;
    uint8_t degrees_per_position;
    char buf_[64];
    char *entity_name;
};