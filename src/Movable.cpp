#include <cassert>

#include "Skin.hpp"
#include "SubEngine/PhysicsSimulator.hpp"
#include "Actor/Actor.hpp"
#include "Component/Movable.hpp"

using namespace Game::Component;

void Movable::postConstruct() noexcept{
    Interface::postConstruct();

    attr=std::make_shared<Skin::Attribute_2D>();

    assert(!owner.expired());
    auto query=owner.lock()->query(SubEngine::Type::PhysicsSimulator);

    if(query.has_value()){
        auto ps=std::static_pointer_cast<
            SubEngine::PhysicsSimulator
        >(query.value());

        ps->appendMovable(std::static_pointer_cast<Movable>(
            owner.lock()->find(Type::Movable)
        ));
    }
}
