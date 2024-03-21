#pragma once

#include "locator.hpp"
#include "System/Logging/Logger.hpp"
#include "System/Managing/Actors.hpp"
#include "System/Input/InputReceiver.hpp"
#include "System/Graphic/Graphics.hpp"
#include "Engine/Core.hpp"

namespace Getter{
    Actors& actors();
    Logger& logger();
    InputReceiver& input();
    Graphics& graphics();
    Core& core();
}

namespace SystemLocator{
    bool isTest();
}