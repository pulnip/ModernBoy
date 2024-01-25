#include <cassert>

#include "Skin.hpp"
#include "SubEngine/PhysicsSimulator.hpp"
#include "Actor/Actor.hpp"
#include "Component/MoveComponent.hpp"

void MoveComponent::injectDependency() noexcept{
    assert(!owner.expired());
    std::shared_ptr<IActor> actor=owner.lock();

    auto query=actor->query(SubEngineName::PhysicsSimulator);

    assert(query.has_value());
    auto ps=std::static_pointer_cast<PhysicsSimulator>(query.value());

    ps->appendMovable(
        std::static_pointer_cast<MoveComponent>(shared_from_this())
    );
}
