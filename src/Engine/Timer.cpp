#include "Engine/Timer.hpp"
#include "Engine/Logger.hpp"

using namespace Engine;

Timer::Timer() noexcept:
    logger(std::make_unique<BindedLogger>("Timer", "Base"))
{
    logger->debug("constructed");
}

Timer::~Timer(){
    logger->debug("destructed");
}