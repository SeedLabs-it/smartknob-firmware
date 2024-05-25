#pragma once
#include "display/page_manager.h"

enum HassOnboardingPages
{
    CONNECT_QRCODE,
    WEBSERVER_QRCODE,
    DEMO_PAGE,
    ABOUT_PAGE,
    ONBOARDING_FLOW_PAGE_COUNT
};

class HassOnboardingPageManager : public PageManager<HassOnboardingPages>
{
};