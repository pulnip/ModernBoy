#include <cassert>
#include <vector>

#include "ActorManager.hpp"
#include "AnimSpriteComponent.hpp"
#include "Ball.hpp"
#include "CollisionComponent.hpp"
#include "Component.hpp"
#include "GraphicsEngineWithSDL.hpp"
#include "MoveComponent.hpp"
#include "PubSubMessage.hpp"
#include "ResourceManagerWithSDL.hpp"

Ball::Ball(const std::weak_ptr<ActorManager> owner) noexcept
    : Actor(owner) {
    position = {1024.0f / 2, 768.0f / 2};
    scale = 5.0f;
}
void Ball::updateActor(const float &deltaTime) noexcept {
    assert(!owner.expired() && "owner: expired");
    // position += velocity * deltaTime;

    if (position.x < -getSize().x) {
        Observable<GameStatus>::notify(GameStatus::GAME_OVER);
    }
}
void Ball::allowCollision(const std::weak_ptr<Actor> opponent) noexcept {
    cc->allow(opponent);
}
void Ball::postConstruct() noexcept {
    auto self = weak_from_this();

    sc = Component::make<AnimSpriteComponent>(self);
    cc = Component::make<CollisionComponent>(self);
    mc = Component::make<MoveComponent>(self);

    mc->velocity = {-200.0f, 235.0f};
    sc->Observable<SDL_Sprite>::subscribe(std::dynamic_pointer_cast<GraphicsEngineWithSDL>(
        owner.lock()->requestSubEngine(SubEngineName::GraphicsEngine).value().lock()));

    if (owner.expired()) {
        return;
    }
    auto o = owner.lock()->requestSubEngine(SubEngineName::ResourceManager);

    if (!o.has_value()) {
        return;
    }
    auto wpManager = o.value();

    if (o.value().expired()) {
        return;
    }
    auto manager = std::dynamic_pointer_cast<ResourceManagerWithSDL>(wpManager.lock());

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