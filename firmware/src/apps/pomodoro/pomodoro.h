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
    #ifdef USE_DISPLAY_BUFFER
        PomodoroApp();
        void render();
    #else
        PomodoroApp(TFT_eSprite *spr_);
        TFT_eSprite *render();
    #endif
    EntityStateUpdate update(AppState state);

    

protected:
    int8_t navigationNext();

private:
    PomodoroState state;
    long startTime;
};