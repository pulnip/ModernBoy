#include <algorithm>
#include <cassert>

#include "Actors.hpp"
#include "Components.hpp"
#include "Game.hpp"
#include "Math.hpp"

Actor::Actor(
    const std::weak_ptr<class Game> game
) noexcept :game(game){
    assert(!game.expired() && "game: expired");
    game.lock()->appendActor(
        std::make_shared<Actor>(this)
    );
}

void Actor::processInput(const uint8_t* keyState) noexcept{
    if(state==EActive){
        for(auto& comp: components){
            comp->processInput(keyState);
        }
        processActorInput(keyState);
    }
}
void Actor::update(const float deltaTime) noexcept{
    updateComponents(deltaTime);
    updateActor(deltaTime);
}
void Actor::updateComponents(const float deltaTime) noexcept{
    if(!isOrdered){
        orderComponents();
    }
    for(auto& component: components){
        component->update(deltaTime);
    }
}

void Actor::appendComponent(const std::shared_ptr<class Component>& component) noexcept{
    components.emplace_back(component);
    isOrdered=false;
}
std::weak_ptr<Component> Actor::queryComponent(const Component* component) const noexcept{
    return *find(component);
}
void Actor::removeComponent(const Component* component) noexcept{
    components.erase(find(component));
}
void Actor::orderComponents() noexcept{
    std::sort(components.begin(), components.end(),
        [](const auto& lhs, const auto& rhs)->bool{
            return lhs->getUpdateOrder() < rhs->getUpdateOrder();
        }
    );
    isOrdered=true;
}
decltype(Actor::components)::const_iterator Actor::find(const Component* component) const noexcept{
    return std::find_if(components.cbegin(), components.cend(),
        [](const auto& lhs, const auto& rhs)->bool{
            return lhs->getUpdateOrder() == rhs->getUpdateOrder();
        }
    );
}

// Real Actors

Paddle::Paddle(Game* game)
:Actor(game), fixed_velocity_y{300}{
    mPosition.x=15.0f;
    mPosition.y=384.0f;
    mVelocity.y=fixed_velocity_y;

    auto bc=new BoxComponent(this);
    bc->SetTexture(new Color(), 15.0f, 120.0f);
    mSize=&bc->GetSize();
    cc=new CollisionComponent(this);
    auto coc=new ControlComponent(this);
}

void Paddle::UpdateActorFirst(float deltaTime){
    mVelocity.y = fixed_velocity_y;
}

void Paddle::UpdateActorLast(float deltaTime){
    mPosition += deltaTime * mVelocity;
}

void Paddle::CollideAllow(Actor* opponent){
    cc->allow(opponent);
}

Wall::Wall(Game* game, int x, int y, int w, int h):Actor(game){
    mPosition.x=x;
    mPosition.y=y;

    auto bc=new BoxComponent(this);
    bc->SetTexture(new Color(), w, h);
    mSize=&bc->GetSize();
}

Ball::Ball(Game* game, int x, int y, int w, int h):Actor(game){
    mPosition.x=x;
    mPosition.y=y;
    mVelocity.x=-200.0f;
    mVelocity.y=235.0f;

    auto sc=new AnimSpriteComponent(this);
    std::vector<SDL_Texture*> anims;
    anims.emplace_back(mGame->GetTexture("../resource/pigeon_1.png"));
    anims.emplace_back(mGame->GetTexture("../resource/pigeon_2.png"));
    anims.emplace_back(mGame->GetTexture("../resource/pigeon_3.png"));
    anims.emplace_back(mGame->GetTexture("../resource/pigeon_2.png"));
    sc->SetAnimTextures(anims);
    mSize=&sc->GetSize();
    mScale=5.0f;
    // auto bc=new BoxComponent(this);
    // bc->SetTexture(new Color(), w, h);
    // mSize=&bc->GetSize();
    cc=new CollisionComponent(this);
}

void Ball::updateActorLast(float deltaTime){
    mPosition += deltaTime * mVelocity;
    
    if(mPosition.x < -mSize->x){
        getGame()->mIsRunning=false;
    }
}

void Ball::collideAllow(Actor* opponent){
    cc->allow(opponent);
}

Ship::Ship(Game* game)
:Actor(game) {
    setPosition({500, 500});
    AnimSpriteComponent* asc = new AnimSpriteComponent(this);
    std::vector<SDL_Texture*> anims={
        mGame->GetTexture("../resource/Ship01.png"),
        mGame->GetTexture("../resource/Ship02.png"),
        mGame->GetTexture("../resource/Ship03.png"),
        mGame->GetTexture("../resource/Ship04.png")
    };
    asc->SetAnimTextures(anims);

    InputComponent* ic=new InputComponent(this);
    ic->setForwardMoveSpeed(300.0f);
    ic->setAngularMoveSpeed(Math::PI);
    ic->setForwardKey(SDL_SCANCODE_D);
    ic->setBackwardKey(SDL_SCANCODE_A);
    ic->setClockwiseKey(SDL_SCANCODE_E);
    ic->setCounterClockwiseKey(SDL_SCANCODE_Q);
}

Asteroid::Asteroid(Game* game):Actor(game){
    SetPosition(Vector2{
        static_cast<float>(Math::random(0, 1024)),
        static_cast<float>(Math::random(0, 768))
    });
    SetRotation(Math::random(0, 1024)/Math::PI);

    SpriteComponent* sc=new SpriteComponent(this);
    sc->SetTexture(game->GetTexture("../resource/Asteroid.png"));
    MoveComponent* mc=new MoveComponent(this);
    mc->SetForwardSpeed(150.0f);
}