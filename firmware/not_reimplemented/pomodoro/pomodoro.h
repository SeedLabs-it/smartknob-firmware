#include "../app.h"
#include <chrono>

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

    TFT_eSprite *render();

protected:
    int8_t navigationNext();

private:
    PomodoroState state;
    long startTime;
};