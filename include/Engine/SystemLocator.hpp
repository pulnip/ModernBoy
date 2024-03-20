#pragma once

#include "locator.hpp"
#include "System/Logging/Logger.hpp"
#include "System/Managing/Actors.hpp"
#include "System/Input/InputReceiver.hpp"
#include "System/Graphic/Graphics.hpp"

namespace Getter{
    Actors& actors();
    Logger& logger();
    InputReceiver& input();
    Graphics& graphics();
}

namespace SystemLocator{
    bool isTest();
}