#include <algorithm>
#include <cassert>

#include <SDL2/SDL.h>

#include "Actors.hpp"
#include "Components.hpp"

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
    componentMap[component->getName()]=component;
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

// Real Actors

Paddle::Paddle(const std::weak_ptr<Game> game) noexcept: Actor(game){
    position={15.0f, 384.0f};
}
void Paddle::collideAllow(const std::weak_ptr<Actor> opponent) noexcept{
    cc->allow(opponent);
}
void Paddle::load(const std::weak_ptr<Actor> self) noexcept{
    bc=Component::Factory::make<BoxComponent>(self);
    cc=Component::Factory::make<CollisionComponent>(self);
    ic=Component::Factory::make<InputComponent>(self);
    appendComponent(bc);
    appendComponent(cc);
    appendComponent(ic);

    bc->setTexture({}, {15.0f, 120.0f});

    ic->velocity()={0, 300.0f};
}

Wall::Wall(const std::weak_ptr<Game> game) noexcept: Actor(game){}
void Wall::load(const std::weak_ptr<Actor> self) noexcept{
    bc=Component::Factory::make<BoxComponent>(self);
    appendComponent(bc);
}

Ball::Ball(const std::weak_ptr<Game> game) noexcept: Actor(game){
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
    sc=Component::Factory::make<AnimSpriteComponent>(self);
    cc=Component::Factory::make<CollisionComponent>(self);
    mc=Component::Factory::make<MoveComponent>(self);
    appendComponent(sc);
    appendComponent(cc);
    appendComponent(mc);

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
    sc=Component::Factory::make<AnimSpriteComponent>(self);
    ic=Component::Factory::make<InputComponentP>(self);
    appendComponent(sc);
    appendComponent(ic);

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
    sc=Component::Factory::make<AnimSpriteComponent>(self);
    mc=Component::Factory::make<AngularMoveComponent>(self);
    appendComponent(sc);
    appendComponent(mc);

    sc->setTexture(game.lock()->getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Asteroid.png"));

    mc->setForwardSpeed(150.0f);
}