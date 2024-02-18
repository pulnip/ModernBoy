#include <cassert>

#include "Component/Movable.hpp"
#include "AnimSpriteComponent.hpp"
#include "Asteroid.hpp"
#include "SubEngine/ActorManager.hpp"
#include "ResourceManagerWithSDL.hpp"


void Asteroid::injectDependency() noexcept {
    auto self = weak_from_this();

    auto mc = Component::make<Movable>(self);
    auto sc = Component::make<AnimSpriteComponent>(self);

    mc->attr().position.linear={
        static_cast<float>(Math::random(0, 1024)),
        static_cast<float>(Math::random(0, 768))
    };
    mc->attr().position.rotation=Math::random(0, 1024) / Math::PI;
    mc->attr().velocity.linear=Vector2::forward(Math::random(-Math::PI, Math::PI)) * Math::random(0, 300);
    mc->attr().velocity.rotation=Math::random(-Math::PI / 2, Math::PI / 2);
    mc->attr().volume.base={64.0f, 64.0f};

    assert(!owner.expired());
    auto query=owner.lock()->query(SubEngine::Type::ResourceManager);

    assert(query.has_value());
    auto manager=std::dynamic_pointer_cast<ResourceManagerWithSDL>(query.value());

    auto q=manager->getTexture("Asteroid.png");

    assert(q.has_value());
    sc->setTexture(q.value());
}