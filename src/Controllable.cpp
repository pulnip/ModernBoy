#include <cassert>

#include "SubEngine/InputSystem.hpp"
#include "Actor/Actor.hpp"
#include "Component/Controllable.hpp"

using namespace Game;
using namespace Game::Component;

void Controllable::setKey(
    const uint8_t key,
    std::function<void(void)> OnPressed,
    std::function<void(void)> OnReleased
) noexcept{
    assert(!controller.expired());
    controller.lock()->registerKey(
        key, std::static_pointer_cast<Controllable>(shared_from_this())
    );

    ifPressed[key] = OnPressed;
    ifReleased[key] = OnReleased;
}

void Controllable::onNotify(const Skin::Key& key) noexcept {
    if(key.status==Skin::Key::Status::PRESSED){
        auto it=ifPressed.find(key.key);
        assert(it != ifPressed.end());
        it->second();
    } else{
        auto it=ifReleased.find(key.key);
        assert(it != ifReleased.end());
        it->second();
    }
}

void Controllable::postConstruct() noexcept{
    Interface::postConstruct();

    setController();
}

void Controllable::setController() noexcept{
    assert(!owner.expired());
    auto query=owner.lock()->query(SubEngine::Type::InputSystem);
    
    if(query.has_value()){
        controller=std::dynamic_pointer_cast<
            SubEngine::InputSystem
        >(query.value());        
    }
}