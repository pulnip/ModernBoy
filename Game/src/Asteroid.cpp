#include "SubEngine/ActorManager.hpp"
#include "ResourceManagerWithSDL.hpp"

#include "Component/MoveComponent.hpp"
#include "AnimSpriteComponent.hpp"

#include "Asteroid.hpp"

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

    if (owner.expired()) return;
    auto actorManager=owner.lock();
    auto query=actorManager->query(SubEngineName::ResourceManager);

    if (!query.has_value()) return;
    if (query.value()==nullptr) return;

    auto manager = std::dynamic_pointer_cast<ResourceManagerWithSDL>(query.value());
    auto oTexture = manager->getTexture("Asteroid.png");

    if (!oTexture.has_value())
    {
        return;
    }
    sc->setTexture(oTexture.value());
}