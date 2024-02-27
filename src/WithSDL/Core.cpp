#include "WithSDL/Core.hpp"
#include "WithSDL/Timer.hpp"
#include "WithSDL/Logger.hpp"
#include "WithSDL/Graphics.hpp"
#include "WithSDL/InputSystem.hpp"
#include "WithSDL/ResourceManager.hpp"

using namespace WithSDL;

Core::Core() noexcept:
    Engine::Core(std::make_unique<Timer>())
{
    Graphics::base=std::make_shared<Graphics>();
    Graphics::get()->initialize();
    ResourceManager::base=std::make_shared<ResourceManager>();
    InputSystem::base=std::make_shared<InputSystem>();
}

Core::~Core(){
    InputSystem::base=nullptr;
    ResourceManager::base=nullptr;
    Graphics::get()->destroyAll();
    Graphics::base=nullptr;
}