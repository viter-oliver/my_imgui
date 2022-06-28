#pragma once
#include "application.h"
class afg_engine : public zl_future::application
{
    void resLoaded();
    void onUpdate();
};