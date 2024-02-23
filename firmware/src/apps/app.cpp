#include "app.h"

#ifdef USE_DISPLAY_BUFFER
void App::render(){}
#else
TFT_eSprite *App::render()
{
    return spr_;
}
#endif

EntityStateUpdate App::updateStateFromKnob(PB_SmartKnobState state)
{
    return EntityStateUpdate();
}

void App::updateStateFromSystem(AppState state)
{
}

int8_t App::navigationNext()
{
    return next;
}

void App::setNext(int8_t next)
{
    this->next = next;
}

int8_t App::navigationBack()
{
    return back;
}

void App::setBack(int8_t back)
{
    this->back = back;
}