#include "../apps.h"

enum OnboardingIds : int8_t
{
    PRINTER_CHAMBER = 0,
    BLINDS = 1,
    CLIMATE = 2,
    LIGHT_DIMMER = 3,
    LIGHT_SWITCH = 4,
    MUSIC = 5,
    SETTINGS = 6,
    STOPWATCH = 7,
    HASS_SETUP_APP = 8,
    POMODORO = 9,
    DEMO = INT8_MAX,
};
class Onboarding : public Apps
{
public:
    Onboarding(){};
    Onboarding(TFT_eSprite *spr_);
};