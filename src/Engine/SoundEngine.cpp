#include "Engine/Core.hpp"
#include "Engine/SoundEngine.hpp"
#include "Engine/Logger.hpp"

using namespace Engine;

SoundEngine::SoundEngine() noexcept:
    logger(std::make_unique<BindedLogger>("SoundEngine", "Base"))
{
    logger->debug("constructed");
}

SoundEngine::~SoundEngine(){
    logger->debug("destructed");
}