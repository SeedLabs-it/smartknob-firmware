#include "../app.h"
#include <chrono>

#include "../../font/NDS1210pt7b.h"

enum class PomodoroState
{
    IDLE,
    WORK,
    BREAK
};

class PomodoroApp : public App
{
public:
    PomodoroApp(TFT_eSprite *spr_);
    EntityStateUpdate update(AppState state);
    EntityStateUpdate updateStateFromKnob(PB_SmartKnobState state);

    TFT_eSprite *render();

protected:
    int8_t navigationNext();

private:
    PomodoroState state;
    boolean state_changed = false;
    long start_time;
};