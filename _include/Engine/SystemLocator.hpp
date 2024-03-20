#pragma once

#include "locator.hpp"
#include "System/Logging/Logger.hpp"
#include "System/Managing/Actors.hpp"
#include "System/Input/InputReceiver.hpp"

namespace Getter{
    Actors& actors();
    Logger& logger();
    InputReceiver& input();
}

namespace SystemLocator{
    bool isTest();
}