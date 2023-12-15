#include <algorithm>

#include "Actors.hpp"
#include "Components.hpp"
#include "Game.hpp"
#include "Math.hpp"

Actor::Actor(Game* game):mGame(game){
    mGame->AddActor(this);
}
Actor::~Actor(){
    mGame->RemoveActor(this);
    for(auto component: mComponents){
        delete component;
    }
}

void Actor::Update(float deltaTime){
    UpdateActorFirst(deltaTime);
    UpdateComponents(deltaTime);
    UpdateActorLast(deltaTime);
}

void Actor::UpdateComponents(float deltaTime){
    auto sortByUpdateOrder=mComponents;
    std::sort(
        sortByUpdateOrder.begin(), sortByUpdateOrder.end(),
        [](Component* lhs, Component* rhs)->bool{
            return lhs->getUpdateOrder() < rhs->getUpdateOrder();
        }
    );
    for(auto component: sortByUpdateOrder){
        component->update(deltaTime);
    }
}

void Actor::ProcessInput(const uint8_t* keyState){
    if(mState==EActive){
        for(auto comp: mComponents){
            comp->processInput(keyState);
        }
        ActorInput(keyState);
    }
}

void Actor::appendComponent(Component* component){
    mComponents.emplace_back(component);
}

void Actor::removeComponent(Component* component){
    mComponents.erase(
        std::find(mComponents.cbegin(), mComponents.cend(), component)
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