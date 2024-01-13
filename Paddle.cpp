#include <SDL2/SDL_keyboard.h>

#include "Actor/Paddle.hpp"
#include "Component/BoxComponent.hpp"
#include "Component/CollisionComponent.hpp"
#include "Component/Component.hpp"
#include "Component/InputComponent.hpp"
#include "Component/MoveComponent.hpp"
#include "SubEngine/ActorManager.hpp"
#include "SubEngine/GraphicsEngine.hpp"
#include "SubEngine/SubEngine.hpp"

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
