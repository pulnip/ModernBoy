#include <SDL2/SDL_keyboard.h>

#include "ActorManager.hpp"
#include "BoxComponent.hpp"
#include "CollisionComponent.hpp"
#include "Component.hpp"
#include "GraphicsEngine.hpp"
#include "InputComponent.hpp"
#include "MoveComponent.hpp"
#include "Paddle.hpp"
#include "SubEngine.hpp"

void Paddle::updateActor(const float &deltaTime) noexcept {
    mc->velocity.y = 0;
}

Paddle::Paddle(const std::weak_ptr<ActorManager> owner) noexcept
    : Actor(owner) {
    position = {15.0f, 384.0f};
}

void Paddle::allowCollision(const std::weak_ptr<Actor> opponent) noexcept {
    cc->allow(opponent);
}

void Paddle::postConstruct() noexcept {
    auto self = weak_from_this();

    bc = Component::make<BoxComponent>(self);
    cc = Component::make<CollisionComponent>(self);
    ic = Component::make<InputComponent>(self);
    mc = Component::make<MoveComponent>(self);

    bc->setTexture({}, {15.0f, 120.0f});
    bc->Observable<ColorRect>::subscribe(std::dynamic_pointer_cast<GraphicsEngine>(
        owner.lock()->requestSubEngine(SubEngineName::GraphicsEngine).value().lock()));
    ic->setKey(SDL_SCANCODE_S, [&v_y = mc->velocity.y]() {
        v_y += 300.0f;
    });
    ic->setKey(SDL_SCANCODE_W, [&v_y = mc->velocity.y]() {
        v_y += -300.0f;
    });
}
