#include "app.h"

TFT_eSprite *App::render()
{
    return spr_;
}

EntityStateUpdate App::updateStateFromKnob(PB_SmartKnobState state)
{
    return EntityStateUpdate();
}

void App::updateStateFromHASS(MQTTStateUpdate mqtt_state_update)
{
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