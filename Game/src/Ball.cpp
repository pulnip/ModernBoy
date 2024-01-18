#include <cassert>
#include <vector>

#include "PubSubMessage.hpp"

#include "Component/CollisionComponent.hpp"
#include "Component/Component.hpp"
#include "Component/MoveComponent.hpp"
#include "SubEngine/ActorManager.hpp"

#include "AnimSpriteComponent.hpp"
#include "GraphicsEngineWithSDL.hpp"
#include "ResourceManagerWithSDL.hpp"

#include "Ball.hpp"

void Ball::updateActor(const float &deltaTime) noexcept {
    assert(!owner.expired() && "owner: expired");
    // position += velocity * deltaTime;

    auto query=queryComponent(ComponentName::MoveComponent);
    assert(query.has_value() && !query.value().expired());
    auto mc=std::dynamic_pointer_cast<MoveComponent>(query.value().lock());

    if (mc->position().x < mc->baseSize().x) {
        Observable<GameStatus>::notify(GameStatus::GAME_OVER);
    }
}
void Ball::allowCollision(const std::weak_ptr<Actor> opponent) noexcept {
    cc->allow(opponent);
}
void Ball::injectDependency() noexcept {
    auto self = weak_from_this();

    sc = Component::make<AnimSpriteComponent>(self);
    cc = Component::make<CollisionComponent>(self);
    mc = Component::make<MoveComponent>(self);

    mc->position={1024.0f / 2, 768.0f / 2};
    mc->scale=5.0f;
    mc->velocity={-200.0f, 235.0f};
    
    sc->Observable<SpriteForSDL>::subscribe(std::dynamic_pointer_cast<GraphicsEngineWithSDL>(
        owner.lock()->query(SubEngineName::GraphicsEngine).value()));

    if (owner.expired()) {
        return;
    }
    auto o = owner.lock()->query(SubEngineName::ResourceManager);

    if (!o.has_value()) return;
    if (o.value() == nullptr) return;
    auto manager = std::dynamic_pointer_cast<ResourceManagerWithSDL>(o.value());

    std::vector<std::optional<SDL_Texture *>> opAnims = {
        manager->getTexture("pigeon_1.png"),
        manager->getTexture("pigeon_2.png"),
        manager->getTexture("pigeon_3.png"),
        manager->getTexture("pigeon_2.png")};

    std::vector<SDL_Texture *> anims;
    for (auto &o : opAnims) {
        if (o.has_value()) {
            anims.emplace_back(o.value());
        }
    }

    sc->setAnimTextures(anims);
}