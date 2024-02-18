#include <cassert>

#include "SubEngine/ActorManager.hpp"
#include "Actor/Actor.hpp"
#include "Component/Movable.hpp"

using namespace Game;
using namespace Game::Actor;

void Interface::update(const Time& deltaTime) noexcept{
    if (state!=State::EActive) return;

    for (auto& comp: orderedComponents) {
        comp->update(deltaTime);
    }
    updateActor(deltaTime);
}

Part Interface::find(const Component::Type name) noexcept {
    auto result=components.find(name);
    if(result==components.end()){
        return nullptr;
    }
    return result->second;
}

std::optional<Plugin> Interface::query(
    const SubEngine::Type name
) noexcept{
    assert(!owner.expired());
    auto manager=owner.lock();

    return manager->query(name);
}

void Interface::add(Part comp) noexcept{
    components.emplace(comp->getType(), comp);
    orderedComponents.emplace(comp);
}

void Interface::remove(Part comp) noexcept{
    components.erase(comp->getType());
    orderedComponents.erase(comp);
}

bool Interface::UpdateOrder::operator()(
    const Part& lhs, const Part& rhs
) const noexcept{
    return lhs->getUpdateOrder() < rhs->getUpdateOrder();
}
