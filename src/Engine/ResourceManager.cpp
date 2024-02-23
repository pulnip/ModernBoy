#include "Engine/Core.hpp"
#include "Engine/ResourceManager.hpp"
#include "Engine/Logger.hpp"

using namespace Engine;

ResourceManager::ResourceManager() noexcept:
    logger(std::make_unique<BindedLogger>("ResourceManager", "Base"))
{
    logger->debug("constructed");
}

ResourceManager::~ResourceManager(){
    logger->debug("destructed");
}
