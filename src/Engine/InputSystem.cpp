#include "Engine/GameLogic.hpp"
#include "Engine/InputSystem.hpp"
#include "Engine/Logger.hpp"

using namespace Engine;

InputSystem::InputSystem() noexcept:
    logger(std::make_unique<BindedLogger>("InputSystem", "Base"))
{
    logger->debug("constructed");
}

InputSystem::~InputSystem(){
    logger->debug("destructed");
}

void InputSystem::connect() noexcept{
    subscribe(GameLogic::get());
}

void InputSystem::registerKey(
    const uint8_t key,
    const std::weak_ptr<Observer<Skin::Key>> subscriber
) noexcept{
    static const auto dummy=Observable<Skin::Key>();
    
    auto [it, loaded]=keyMap.try_emplace(key, dummy);

    it->second.subscribe(subscriber);
}