#include <algorithm>

#include "Actors.hpp"
#include "Components.hpp"
#include "Game.hpp"

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
            return lhs->GetUpdateOrder() < rhs->GetUpdateOrder();
        }
    );
    for(auto component: sortByUpdateOrder){
        component->Update(deltaTime);
    }
}

void Actor::AddComponent(Component* component){
    mComponents.emplace_back(component);
}

void Actor::RemoveComponent(Component* component){
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
    cc->Allow(opponent);
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
    anims.emplace_back(mGame->GetTexture("resource/pigeon_1.png"));
    anims.emplace_back(mGame->GetTexture("resource/pigeon_2.png"));
    anims.emplace_back(mGame->GetTexture("resource/pigeon_3.png"));
    anims.emplace_back(mGame->GetTexture("resource/pigeon_2.png"));
    sc->SetAnimTextures(anims);
    mSize=&sc->GetSize();
    mScale=5.0f;
    // auto bc=new BoxComponent(this);
    // bc->SetTexture(new Color(), w, h);
    // mSize=&bc->GetSize();
    cc=new CollisionComponent(this);
}

void Ball::UpdateActorLast(float deltaTime){
    mPosition += deltaTime * mVelocity;
    
    if(mPosition.x < -mSize->x){
        GetGame()->mIsRunning=false;
    }
}

void Ball::CollideAllow(Actor* opponent){
    cc->Allow(opponent);
}

Ship::Ship(Game* game)
:Actor(game) {
    SetPosition({500, 500});
    AnimSpriteComponent* asc = new AnimSpriteComponent(this);
    std::vector<SDL_Texture*> anims={
        mGame->GetTexture("resource/Ship01.png"),
        mGame->GetTexture("resource/Ship02.png"),
        mGame->GetTexture("resource/Ship03.png"),
        mGame->GetTexture("resource/Ship04.png")
    };
    asc->SetAnimTextures(anims);

    ControlComponent* conc = new ControlComponent(this);
}

void Ship::UpdateActorLast(float deltaTime){
    mPosition += deltaTime * mVelocity;
}