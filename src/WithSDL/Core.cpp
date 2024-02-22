#include "WithSDL/Core.hpp"
#include "WithSDL/Timer.hpp"
#include "WithSDL/Logger.hpp"
#include "WithSDL/Graphics.hpp"
#include "WithSDL/InputSystem.hpp"

using namespace WithSDL;

Core::Core() noexcept:
    Engine::Core(std::make_unique<Timer>())
{
    Graphics::base=std::make_shared<Graphics>();
    InputSystem::base=std::make_shared<InputSystem>();
}

Core::~Core(){}