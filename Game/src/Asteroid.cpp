#include "Component/Component.hpp"
#include "Component/MoveComponent.hpp"
#include "SubEngine/ActorManager.hpp"

#include "AnimSpriteComponent.hpp"
#include "ResourceManagerWithSDL.hpp"

#include "Asteroid.hpp"

void func() {

}

Asteroid::Asteroid() noexcept{
}
void Asteroid::injectDependency() noexcept {
    auto self = weak_from_this();

    sc = Component::make<AnimSpriteComponent>(self);
    mc = Component::make<MoveComponent>(self);

    mc->position={
        static_cast<float>(Math::random(0, 1024)),
        static_cast<float>(Math::random(0, 768))
    };
    mc->rotation=Math::random(0, 1024) / Math::PI;
    mc->velocity=Vector2::forward(Math::random(-Math::PI, Math::PI)) * Math::random(0, 300);
    mc->rotationVelocity=Math::random(-Math::PI / 2, Math::PI / 2);

    if (owner.expired())
    {
        return;
    }
    auto o = owner.lock()->requestSubEngine(SubEngineName::ResourceManager);

    if (!o.has_value())
    {
        return;
    }
    auto wpManager = o.value();

    if (o.value().expired())
    {
        return;
    }
    auto manager = std::dynamic_pointer_cast<ResourceManagerWithSDL>(wpManager.lock());
    auto oTexture = manager->getTexture("Asteroid.png");

    if (!oTexture.has_value())
    {
        return;
    }
    sc->setTexture(oTexture.value());
}