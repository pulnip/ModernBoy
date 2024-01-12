#include <algorithm>
#include <cassert>
#include <ranges>

#include <SDL2/SDL.h>

#include "ActorManager.hpp"
#include "Actors.hpp"
#include "Components.hpp"
#include "GraphicsEngineWithSDL.hpp"
#include "ResourceManagerWithSDL.hpp"

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

void Actor::onNotify(MSG_t msg, spObservable comp) noexcept {
    switch (msg) {
    case Lifetime::CONSTRUCTED:
        appendComponent(comp);
        break;
    case Lifetime::DESTRUCTED:
        components.erase(comp->getName());
        break;
    default:
        assert(false);
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

Actor::Actor(const std::weak_ptr<ActorManager> owner) noexcept : owner(owner) {
    subscribe(owner);
}

void Actor::postConstruct() noexcept {
    notify(Lifetime::CONSTRUCTED);
}

void Actor::updateComponents(const float &deltaTime) noexcept {
    for (auto &comp : orderedComponents) {
        comp->update(deltaTime);
    }
}

bool Actor::UpdateOrder::operator()(const ptr &lhs, const ptr &rhs) const {
    return lhs->getUpdateOrder() < rhs->getUpdateOrder();
}

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
    bc->Observable<ColorRect>::subscribe(std::dynamic_pointer_cast<GraphicsEngine>(
        owner.lock()->requestSubEngine(SubEngineName::GraphicsEngine).value().lock()));
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

    bc->Observable<ColorRect>::subscribe(std::dynamic_pointer_cast<GraphicsEngine>(
        owner.lock()->requestSubEngine(SubEngineName::GraphicsEngine).value().lock()));
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
        manager->getTexture("Ship01.png"),
        manager->getTexture("Ship02.png"),
        manager->getTexture("Ship03.png"),
        manager->getTexture("Ship04.png")};

    std::vector<SDL_Texture *> anims;
    for (auto &o : opAnims) {
        if (o.has_value()) {
            anims.emplace_back(o.value());
        }
    }

    sc->setAnimTextures(anims);
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

    mc->velocity = Vector2::forward(Math::random(-Math::PI, Math::PI)) * Math::random(0, 300);
    mc->rotationVelocity = Math::random(-Math::PI / 2, Math::PI / 2);

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
    auto oTexture = manager->getTexture("Asteroid.png");

    if (!oTexture.has_value()) {
        return;
    }
    sc->setTexture(oTexture.value());
}
