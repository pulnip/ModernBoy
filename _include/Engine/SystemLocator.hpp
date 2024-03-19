#pragma once

#include "locator.hpp"
#include "System/Logging/Logger.hpp"
#include "System/Managing/Actors.hpp"

Actors& __Actors();
Logger& __Logger();

namespace SystemLocator{
    bool isTest();
}