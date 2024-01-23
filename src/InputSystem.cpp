#include <cassert>

#include "GameEngine/GameEngine.hpp"
#include "SubEngine/GameLogic.hpp"
#include "SubEngine/InputSystem.hpp"

void InputSystem::registerKey(
    const uint8_t key,
    const std::weak_ptr<Observer<Key>> subscriber
) noexcept{
    static const auto dummy=Observable<Key>();
    
    auto [it, loaded]=keyMap.try_emplace(key, dummy);

    it->second.subscribe(subscriber);
}

void InputSystem::injectDependency() noexcept{
    assert(!owner.expired());
    std::shared_ptr<IGameEngine> ige=owner.lock();
    auto gl=std::dynamic_pointer_cast<GameLogic>(
        ige->find(SubEngineName::GameLogic)
    );

    assert(gl!=nullptr);
    registerKey(escapeKeycode(), gl);

    setAttribute();
}

#include <iostream>
#include <set>
#include <string>

void InputSystem_default::update(const float& deltaTime) noexcept{
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

        if(*it == key) publisher.notify({Key::Status::PRESSED, key});
    }
}