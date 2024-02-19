#include <cassert>

#include "Component/Movable.hpp"
#include "AnimSprite.hpp"
#include "Asteroid.hpp"
#include "SubEngine/ActorManager.hpp"
#include "ResourceManagerWithSDL.hpp"

using namespace Game;
using namespace My::Math;

void Asteroid::postConstruct() noexcept {
    Interface::postConstruct();

    auto self = weak_from_this();

    auto mc = Component::Interface::make<Component::Movable>(self);
    auto sc = Component::Interface::make<
        WithSDL::Component::AnimSprite
    >(self);

    mc->attr->position.linear={
        static_cast<float>(random(0, 1024)),
        static_cast<float>(random(0, 768))
    };
    mc->attr->position.rotation=random(0, 1024) / Number::PI;
    mc->attr->velocity.linear=forward(
        random(-Number::PI, Number::PI)
    ) * random(0, 300);
    mc->attr->velocity.rotation=random(-Number::PI / 2, Number::PI / 2);
    mc->attr->volume.base={64.0f, 64.0f};

    assert(!owner.expired());
    auto query=owner.lock()->query(SubEngine::Type::ResourceManager);

    assert(query.has_value());
    auto manager=std::dynamic_pointer_cast<
        WithSDL::SubEngine::ResourceManager
    >(query.value());

    auto q=manager->getTexture("Asteroid.png");

    assert(q.has_value());
    sc->setTexture(q.value());
}