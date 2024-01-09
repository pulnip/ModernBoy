#include <algorithm>
#include <cassert>
#include <ranges>

#include <SDL2/SDL.h>

#include "Actors.hpp"
#include "Components.hpp"
#include "SubEngine.hpp"

// Actor interface

void Actor::processInput(const uint8_t *keyState) noexcept {
    if (state != EActive)
        return;

    // Alternative
    // for(auto &[_, comp] : components) {
    for (auto &comp : components | std::views::values) {
        comp->processInput(keyState);
    }
    processActorInput(keyState);
}

void Actor::update(const float &deltaTime) noexcept {
    if (state != EActive)
        return;

    updateComponents(deltaTime);
    updateActor(deltaTime);
}

void Actor::onNotify(std::shared_ptr<Component> comp, PSMSG::Lifetime msg) {
    switch (msg) {
    case PSMSG::Lifetime::CONSTRUCTED:
        appendComponent(comp);
        break;
    case PSMSG::Lifetime::DESTRUCTED:
        components.erase(comp->getName());
    }
}

void Actor::appendComponent(const std::shared_ptr<Component> component) noexcept {
    components.emplace(component->getName(), component);
    orderedComponents.emplace(component);
}
std::optional<std::weak_ptr<Component>>
Actor::queryComponent(const ComponentName name) noexcept {
    auto result = components.find(name);
    if (result == components.end()) {
        return std::nullopt;
    }
    return result->second;
}

Actor::Actor(const std::weak_ptr<ActorManager> owner) noexcept
    : owner(owner) {
}

void Actor::updateComponents(const float &deltaTime) noexcept {
    for (auto &comp : orderedComponents) {
        comp->update(deltaTime);
    }
}

struct Actor::comp_update_order {
    using ptr = std::shared_ptr<Component>;
    bool operator()(const ptr &lhs, const ptr &rhs) const {
        return lhs->getUpdateOrder() < rhs->getUpdateOrder();
    }
};

// Concrete Actor

// Paddle

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

    ic->setKey(SDL_SCANCODE_S, [&v_y = mc->velocity.y]() {
        v_y += 300.0f;
    });
    ic->setKey(SDL_SCANCODE_W, [&v_y = mc->velocity.y]() {
        v_y += -300.0f;
    });
}

// Wall

Wall::Wall(const std::weak_ptr<ActorManager> owner) noexcept
    : Actor(owner) {
}
void Wall::postConstruct() noexcept {
    auto self = weak_from_this();

    bc = Component::make<BoxComponent>(self);
    mc = Component::make<MoveComponent>(self);
}

// Ball

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

    auto manager = owner.lock();
    std::vector<SDL_Texture *> anims = {
        manager->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/pigeon_1.png"),
        manager->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/pigeon_2.png"),
        manager->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/pigeon_3.png"),
        manager->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/pigeon_2.png")};
    sc->setAnimTextures(anims);

    mc->velocity = {-200.0f, 235.0f};
}

// Ship

void Ship::updateActor(const float &deltaTime) noexcept {
    mc->velocity = {0.0f, 0.0f};
    mc->rotationVelocity = 0.0;
}
Ship::Ship(const std::weak_ptr<ActorManager> owner) noexcept
    : Actor(owner) {
    position = {500.0f, 500.0f};
}
void Ship::postConstruct() noexcept {
    auto self = weak_from_this();

    sc = Component::make<AnimSpriteComponent>(self);
    ic = Component::make<InputComponent>(self);
    mc = Component::make<MoveComponent>(self);

    auto _game = owner.lock();
    std::vector<SDL_Texture *> anims = {
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Ship01.png"),
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Ship02.png"),
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Ship03.png"),
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Ship04.png")};
    sc->setAnimTextures(anims);

    ic->setKey(SDL_SCANCODE_Q, [&rv = mc->rotationVelocity]() {
        rv += -Math::PI;
    });
    ic->setKey(SDL_SCANCODE_E, [&rv = mc->rotationVelocity]() {
        rv += Math::PI;
    });
    ic->setKey(SDL_SCANCODE_D, [&v = mc->velocity, &r = rotation]() {
        v += Vector2::forward(r) * 300.0f;
    });
    ic->setKey(SDL_SCANCODE_A, [&v = mc->velocity, &r = rotation]() {
        v += Vector2::forward(r) * -300.0f;
    });
}

// Asteroid

Asteroid::Asteroid(const std::weak_ptr<ActorManager> owner) noexcept
    : Actor(owner) {
    position = {
        static_cast<float>(Math::random(0, 1024)),
        static_cast<float>(Math::random(0, 768))};
    rotation = Math::random(0, 1024) / Math::PI;
}
void Asteroid::postConstruct() noexcept {
    auto self = weak_from_this();

    sc = Component::make<AnimSpriteComponent>(self);
    mc = Component::make<MoveComponent>(self);

    sc->setTexture(owner.lock()->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Asteroid.png"));

    mc->velocity = Vector2::forward(Math::random(-Math::PI, Math::PI)) * Math::random(0, 300);
    mc->rotationVelocity = Math::random(-Math::PI / 2, Math::PI / 2);
}