#include <algorithm>
#include <cassert>

#include <SDL2/SDL.h>

#include "Actors.hpp"
#include "Components.hpp"
#include "Game.hpp"
#include "Math.hpp"

Actor::Actor(const std::weak_ptr<Game> game) noexcept: game(game){
    assert(!game.expired() && "game: expired");
}

void Actor::processInput(const uint8_t* keyState) noexcept{
    if(state!=EActive) return;

    for(auto& comp: components){
        assert(!comp.expired() && "comp: expired");
        comp.lock()->processInput(keyState);
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
        assert(!comp.expired() && "comp: expired");
        comp.lock()->update(deltaTime);
    }
}

void Actor::appendComponent(const std::shared_ptr<Component> component) noexcept{
    components.emplace_back(component);
    isOrdered=false;
}
void Actor::orderComponents() noexcept{
    std::sort(components.begin(), components.end(),
        [](const auto& lhs, const auto& rhs)->bool{
            assert(
                !lhs.expired() &&
                !rhs.expired() &&
                "lhs or rhs: expired"
            );
            return lhs.lock()->getUpdateOrder() < rhs.lock()->getUpdateOrder();
        }
    );
    isOrdered=true;
}

// Real Actors

Paddle::Paddle(const std::weak_ptr<Game> game) noexcept:
Actor(game),
bc(std::make_shared<BoxComponent>(weak_from_this())),
cc(std::make_shared<CollisionComponent>(weak_from_this())),
ic(std::make_shared<InputComponentA>(weak_from_this())){
    appendComponent(bc);
    appendComponent(cc);
    appendComponent(ic);

    position={15.0f, 384.0f};
    bc->setTexture({}, {15.0f, 120.0f});
    ic->setMoveVelocity({0, 300.0f});
}

void Paddle::updateActor(const float deltaTime) noexcept{
    position += deltaTime * velocity;
}

void Paddle::collideAllow(const std::weak_ptr<Actor> opponent) noexcept{
    cc->allow(opponent);
}

Wall::Wall(const std::weak_ptr<Game> game) noexcept:
Actor(game),
bc(std::make_shared<BoxComponent>(weak_from_this())){
    appendComponent(bc);
}

Ball::Ball(const std::weak_ptr<Game> game) noexcept:
Actor(game),
sc(std::make_shared<AnimSpriteComponent>(weak_from_this())),
cc(std::make_shared<CollisionComponent>(weak_from_this())){
    appendComponent(sc);
    appendComponent(cc);

    velocity={-200.0f, 235.0f};
    scale=5.0f;

    auto _game=game.lock();
    std::vector<SDL_Texture*> anims={
        _game->getTexture("../resource/pigeon_1.png"),
        _game->getTexture("../resource/pigeon_2.png"),
        _game->getTexture("../resource/pigeon_3.png"),
        _game->getTexture("../resource/pigeon_2.png")
    };
    sc->setAnimTextures(anims);
}

void Ball::updateActor(const float deltaTime) noexcept{
    assert(!game.expired() && "game: expired");
    position += deltaTime * velocity;
    
    if(position.x < -getSize().x){
        game.lock()->quit();
    }
}

void Ball::collideAllow(const std::weak_ptr<Actor> opponent) noexcept{
    cc->allow(opponent);
}

Ship::Ship(const std::weak_ptr<Game> game) noexcept:
Actor(game){
    auto w=weak_from_this();
    sc=std::make_shared<AnimSpriteComponent>(weak_from_this());
    ic=std::make_shared<InputComponentP>(weak_from_this());
    appendComponent(sc);
    appendComponent(ic);

    position={500.0f, 500.0f};

    auto _game=game.lock();
    std::vector<SDL_Texture*> anims={
        _game->getTexture("../resource/Ship01.png"),
        _game->getTexture("../resource/Ship02.png"),
        _game->getTexture("../resource/Ship03.png"),
        _game->getTexture("../resource/Ship04.png")
    };
    sc->setAnimTextures(anims);

    ic->setForwardMoveSpeed(300.0f);
    ic->setAngularMoveSpeed(Math::PI);
    ic->setForwardKey(SDL_SCANCODE_D);
    ic->setBackwardKey(SDL_SCANCODE_A);
    ic->setClockwiseKey(SDL_SCANCODE_E);
    ic->setCounterClockwiseKey(SDL_SCANCODE_Q);
}

Asteroid::Asteroid(const std::weak_ptr<Game> game) noexcept:
Actor(game),
sc(std::make_shared<AnimSpriteComponent>(weak_from_this())),
mc(std::make_shared<AngularMoveComponent>(weak_from_this())){
    appendComponent(sc);
    appendComponent(mc);

    position={
        static_cast<float>(Math::random(0, 1024)),
        static_cast<float>(Math::random(0, 768))
    };
    rotation = Math::random(0, 1024)/Math::PI;

    auto _game=game.lock();
    sc->setTexture(_game->getTexture("../resource/Asteroid.png"));

    mc->setForwardSpeed(150.0f);
}