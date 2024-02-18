#include <cassert>

#include "Skin.hpp"
#include "SubEngine/PhysicsSimulator.hpp"
#include "Actor/Actor.hpp"
#include "Component/Movable.hpp"

void Movable::injectDependency() noexcept{
    assert(!owner.expired());
    std::shared_ptr<Actor::Interface> actor=owner.lock();

    auto query=actor->query(SubEngine::Type::PhysicsSimulator);

    assert(query.has_value());
    auto ps=std::static_pointer_cast<PhysicsSimulator>(query.value());

    ps->appendMovable(
        std::static_pointer_cast<Movable>(shared_from_this())
    );
}
