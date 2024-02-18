#include <cassert>

#include "GameEngine/GameEngine.hpp"
#include "SubEngine/GameLogic.hpp"
#include "SubEngine/InputSystem.hpp"

using namespace Game;
using namespace Game::SubEngine;

void InputSystem::registerKey(
    const uint8_t key,
    const std::weak_ptr<Observer<Skin::Key>> subscriber
) noexcept{
    static const auto dummy=Observable<Skin::Key>();
    
    auto [it, loaded]=keyMap.try_emplace(key, dummy);

    it->second.subscribe(subscriber);
}

void InputSystem::postConstruct() noexcept{
    assert(!owner.expired());
    subscribe(std::dynamic_pointer_cast<GameLogic>(
        owner.lock()->find(Type::GameLogic)
    ));
}

#include <iostream>
#include <set>
#include <string>

using namespace WithSTD::SubEngine;

void InputSystem_default::update(const Game::Time& deltaTime) noexcept{
    std::set<char> log;
    std::string str;

    std::cin >> str;
    for(char c: str){
        log.emplace(c);
    }

    auto it=log.begin();
    for(auto& [key, publisher]: keyMap){
        while(*it < key) ++it;
        if(it == log.end()) break;

        if(*it == key) publisher.notify({Skin::Key::Status::PRESSED, key});
    }
}