#include <algorithm>
#include <cassert>
#include <unordered_map>

#include <SDL2/SDL.h>

#include "Actors.hpp"
#include "Components.hpp"
#include "Game.hpp"

using cf = Component::Factory;

// Actor interface

Actor::Actor(const std::weak_ptr<Game> game) noexcept
  : game(game)
{
    assert(!game.expired() && "game: expired");
}

void
Actor::processInput(const uint8_t* keyState) noexcept
{
    if (state != EActive)
        return;

    for (auto& comp : components) {
        comp->processInput(keyState);
    }
    processActorInput(keyState);
}
void
Actor::update(const float deltaTime) noexcept
{
    if (state != EActive)
        return;

    updateComponents(deltaTime);
    updateActor(deltaTime);
}
void
Actor::updateComponents(const float deltaTime) noexcept
{
    if (!isOrdered) {
        orderComponents();
    }
    for (auto& comp : components) {
        comp->update(deltaTime);
    }
}

void
Actor::appendComponent(const std::shared_ptr<Component> component) noexcept
{
    components.emplace_back(component);

    static const std::unordered_map<std::string, std::string> m{
        { CollisionComponent::className, CollisionComponent::className },
        { DrawComponent::className, DrawComponent::className },
        { BoxComponent::className, DrawComponent::className },
        { SpriteComponent::className, DrawComponent::className },
        { AnimSpriteComponent::className, DrawComponent::className },
        { BGSpriteComponent::className, DrawComponent::className },
        { MoveComponent::className, MoveComponent::className },
        { InputComponent::className, InputComponent::className }
    };
    const auto name = (*m.find(component->getName())).second;
    componentMap[name] = component;
    isOrdered = false;
}
std::weak_ptr<Component>
Actor::queryComponent(const std::string& name) noexcept
{
    return componentMap[name];
}
void
Actor::orderComponents() noexcept
{
    std::sort(components.begin(), components.end(), [](const auto& lhs, const auto& rhs) -> bool {
        return lhs->getUpdateOrder() < rhs->getUpdateOrder();
    });
    isOrdered = true;
}

// Concrete Actor

// Paddle

void
Paddle::updateActor(const float deltaTime) noexcept
{
    mc->velocity.y = 0;
}
Paddle::Paddle(const std::weak_ptr<Game> game) noexcept
  : Actor(game)
{
    position = { 15.0f, 384.0f };
}
void
Paddle::allowCollision(const std::weak_ptr<Actor> opponent) noexcept
{
    cc->allow(opponent);
}
void
Paddle::load(const std::weak_ptr<Actor> self) noexcept
{
    bc = cf::make<BoxComponent>(self);
    cc = cf::make<CollisionComponent>(self);
    ic = cf::make<InputComponent>(self);
    mc = cf::make<MoveComponent>(self);

    bc->setTexture({}, { 15.0f, 120.0f });

    ic->setKey(SDL_SCANCODE_S, [&v_y = mc->velocity.y]() {
        v_y += 300.0f;
    });
    ic->setKey(SDL_SCANCODE_W, [&v_y = mc->velocity.y]() {
        v_y += -300.0f;
    });
}

// Wall

Wall::Wall(const std::weak_ptr<Game> game) noexcept
  : Actor(game)
{
}
void
Wall::load(const std::weak_ptr<Actor> self) noexcept
{
    bc = cf::make<BoxComponent>(self);
    mc = cf::make<MoveComponent>(self);
}

// Ball

Ball::Ball(const std::weak_ptr<Game> game) noexcept
  : Actor(game)
{
    position = { 1024.0f / 2, 768.0f / 2 };
    scale = 5.0f;
}
void
Ball::updateActor(const float deltaTime) noexcept
{
    assert(!game.expired() && "game: expired");
    // position += velocity * deltaTime;

    if (position.x < -getSize().x) {
        game.lock()->quit();
    }
}
void
Ball::allowCollision(const std::weak_ptr<Actor> opponent) noexcept
{
    cc->allow(opponent);
}
void
Ball::load(const std::weak_ptr<Actor> self) noexcept
{
    sc = cf::make<AnimSpriteComponent>(self);
    cc = cf::make<CollisionComponent>(self);
    mc = cf::make<MoveComponent>(self);

    auto _game = game.lock();
    std::vector<SDL_Texture*> anims = {
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/pigeon_1.png"),
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/pigeon_2.png"),
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/pigeon_3.png"),
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/pigeon_2.png")
    };
    sc->setAnimTextures(anims);

    mc->velocity = { -200.0f, 235.0f };
}

// Ship

void
Ship::updateActor(const float deltaTime) noexcept
{
    mc->velocity = { 0.0f, 0.0f };
    mc->rotationVelocity = 0.0;
}
Ship::Ship(const std::weak_ptr<Game> game) noexcept
  : Actor(game)
{
    position = { 500.0f, 500.0f };
}
void
Ship::load(const std::weak_ptr<Actor> self) noexcept
{
    sc = cf::make<AnimSpriteComponent>(self);
    ic = cf::make<InputComponent>(self);
    mc = cf::make<MoveComponent>(self);

    auto _game = game.lock();
    std::vector<SDL_Texture*> anims = {
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Ship01.png"),
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Ship02.png"),
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Ship03.png"),
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Ship04.png")
    };
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

Asteroid::Asteroid(const std::weak_ptr<Game> game) noexcept
  : Actor(game)
{
    position = {
        static_cast<float>(Math::random(0, 1024)),
        static_cast<float>(Math::random(0, 768))
    };
    rotation = Math::random(0, 1024) / Math::PI;
}
void
Asteroid::load(const std::weak_ptr<Actor> self) noexcept
{
    sc = cf::make<AnimSpriteComponent>(self);
    mc = cf::make<MoveComponent>(self);

    sc->setTexture(game.lock()->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Asteroid.png"));

    mc->velocity = Vector2::forward(Math::random(-Math::PI, Math::PI)) * Math::random(0, 300);
    mc->rotationVelocity = Math::random(-Math::PI / 2, Math::PI / 2);
}