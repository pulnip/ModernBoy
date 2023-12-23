#include <algorithm>
#include <cassert>
#include <unordered_map>

#include <SDL2/SDL.h>

#include "Actors.hpp"
#include "Components.hpp"
#include "Game.hpp"

using cf=Component::Factory;

// Actor interface

Actor::Actor(const std::weak_ptr<Game> game) noexcept: game(game){
    assert(!game.expired() && "game: expired");
}

void Actor::processInput(const uint8_t* keyState) noexcept{
    if(state!=EActive) return;

    for(auto& comp: components){
        comp->processInput(keyState);
    }
    processActorInput(keyState);
}
void Actor::update(const float deltaTime) noexcept{
    if(state!=EActive) return;

    updateComponents(deltaTime);
    updateActor(deltaTime);
}
void Actor::updateComponents(const float deltaTime) noexcept{
    if(!isOrdered){
        orderComponents();
    }
    for(auto& comp: components){
        comp->update(deltaTime);
    }
}

void Actor::appendComponent(const std::shared_ptr<Component> component) noexcept{
    components.emplace_back(component);

    static const std::unordered_map<std::string, std::string> m{
        {CollisionComponent::className, CollisionComponent::className},
        {DrawComponent::className,       DrawComponent::className},
        {BoxComponent::className,        DrawComponent::className},
        {SpriteComponent::className,     DrawComponent::className},
        {AnimSpriteComponent::className, DrawComponent::className},
        {BGSpriteComponent::className,   DrawComponent::className},
        {MoveComponent::className,        MoveComponent::className},
        {AngularMoveComponent::className, MoveComponent::className},
        {InputComponent::className,        InputComponent::className},
        {AngularInputComponent::className, InputComponent::className}
    };
    const auto name=(*m.find(component->getName())).second;
    componentMap[name]=component;
    isOrdered=false;
}
std::weak_ptr<Component> Actor::queryComponent(const std::string& name) noexcept{
    return componentMap[name];
}
void Actor::orderComponents() noexcept{
    std::sort(components.begin(), components.end(),
        [](const auto& lhs, const auto& rhs)->bool{
            return lhs->getUpdateOrder() < rhs->getUpdateOrder();
        }
    );
    isOrdered=true;
}

// Concrete Actor

Paddle::Paddle(const std::weak_ptr<Game> game) noexcept: Actor(game){
    position={15.0f, 384.0f};
}
void Paddle::collideAllow(const std::weak_ptr<Actor> opponent) noexcept{
    cc->allow(opponent);
}
void Paddle::load(const std::weak_ptr<Actor> self) noexcept{
    bc=cf::make<BoxComponent>(self);
    cc=cf::make<CollisionComponent>(self);
    ic=cf::make<InputComponent>(self);
    mc=cf::make<MoveComponent>(self);

    bc->setTexture({}, {15.0f, 120.0f});

    ic->setSpeedPreset(Vector2{0.0f, 300.0f});

    ic->setXPKey(SDL_SCANCODE_D);
    ic->setXNKey(SDL_SCANCODE_A);
    ic->setYPKey(SDL_SCANCODE_S);
    ic->setYNKey(SDL_SCANCODE_W);
}

Wall::Wall(const std::weak_ptr<Game> game) noexcept: Actor(game){}
void Wall::load(const std::weak_ptr<Actor> self) noexcept{
    bc=cf::make<BoxComponent>(self);
    mc=cf::make<MoveComponent>(self);
}

Ball::Ball(const std::weak_ptr<Game> game) noexcept: Actor(game){
    position={1024.0f/2, 768.0f/2};
    scale=5.0f;
}
void Ball::updateActor(const float deltaTime) noexcept{
    assert(!game.expired() && "game: expired");
    // position += velocity * deltaTime;
    
    if(position.x < -getSize().x){
        game.lock()->quit();
    }
}
void Ball::collideAllow(const std::weak_ptr<Actor> opponent) noexcept{
    cc->allow(opponent);
}
void Ball::load(const std::weak_ptr<Actor> self) noexcept{
    sc=cf::make<AnimSpriteComponent>(self);
    cc=cf::make<CollisionComponent>(self);
    mc=cf::make<MoveComponent>(self);

    auto _game=game.lock();
    std::vector<SDL_Texture*> anims={
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/pigeon_1.png"),
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/pigeon_2.png"),
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/pigeon_3.png"),
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/pigeon_2.png")
    };
    sc->setAnimTextures(anims);

    mc->velocity()={-200.0f, 235.0f};
}

Ship::Ship(const std::weak_ptr<Game> game) noexcept:
Actor(game){
    position={500.0f, 500.0f};
}
void Ship::load(const std::weak_ptr<Actor> self) noexcept{
    sc=cf::make<AnimSpriteComponent>(self);
    ic=cf::make<AngularInputComponent>(self);
    mc=cf::make<AngularMoveComponent>(self);

    auto _game=game.lock();
    std::vector<SDL_Texture*> anims={
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Ship01.png"),
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Ship02.png"),
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Ship03.png"),
        _game->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Ship04.png")
    };
    sc->setAnimTextures(anims);

    ic->setForwardSpeedPreset(300.0f);
    ic->setAngularSpeedPreset(Math::PI);
    ic->setForwardKey(SDL_SCANCODE_D);
    ic->setBackwardKey(SDL_SCANCODE_A);
    ic->setClockwiseKey(SDL_SCANCODE_E);
    ic->setCounterClockwiseKey(SDL_SCANCODE_Q);
}

Asteroid::Asteroid(const std::weak_ptr<Game> game) noexcept:
Actor(game){
    position={
        static_cast<float>(Math::random(0, 1024)),
        static_cast<float>(Math::random(0, 768))
    };
    rotation = Math::random(0, 1024)/Math::PI;
}
void Asteroid::load(const std::weak_ptr<Actor> self) noexcept{
    sc=cf::make<AnimSpriteComponent>(self);
    mc=cf::make<AngularMoveComponent>(self);

    sc->setTexture(game.lock()->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Asteroid.png"));

    mc->setForwardSpeed(150.0f);
}