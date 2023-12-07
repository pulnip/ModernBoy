#include <algorithm>

#include "Actors.hpp"
#include "Components.hpp"
#include "Game.hpp"

Actor::Actor(Game* game):game(game){}
Actor::~Actor(){
    game->RemoveActor(this);
    for(auto component: mComponents){
        delete component;
    }
}

void Actor::Update(float deltaTime){
    UpdateActor(deltaTime);
    UpdateComponents(deltaTime);
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
        component->update(deltaTime);
    }
}

void Actor::UpdateActor(float deltaTime){}

void Actor::AddComponent(Component* component){
    mComponents.emplace_back(component);
}

void Actor::RemoveComponent(Component* component){
    mComponents.erase(
        std::find(mComponents.cbegin(), mComponents.cend(), component)
    );
}

// Real Actors

Paddle::Paddle(Game* game):Actor(game)
,start_position{15.0, 768/2}, fixed_velocity{0, 300}, fixed_size{15, 120}{
    tc=new TransformComponent(this,
        start_position.x, start_position.y
    );
    auto sdc=new SimpleDrawComponent(this,
        fixed_size.x, fixed_size.y,
        tc
    );
    game->drawables.emplace_back(sdc);
    cc=new CollisionComponent(this,
        tc, sdc
    );
    auto con_c=new ControlComponent(this,
        tc, game
    );
    AddComponent(tc);
    AddComponent(sdc);
    AddComponent(cc);
    AddComponent(con_c);
}

void Paddle::UpdateActor(float deltaTime){
    tc->velocity.y = fixed_velocity.y;
}

Wall::Wall(Game* game, int x, int y, int w, int h):Actor(game){
    auto tc=new TransformComponent(this, x, y);
    auto sdc=new SimpleDrawComponent(this, w, h, tc);
    game->drawables.emplace_back(sdc);
    cc=new CollisionComponent(this, tc, sdc);

    AddComponent(tc);
    AddComponent(sdc);
    AddComponent(cc);
}

Ball::Ball(Game* game, int x, int y, int w, int h):Actor(game){
    auto tc=new TransformComponent(this, x, y);
    tc->velocity.x = -200.0f;
    tc->velocity.y = 235.0f;
    auto sdc=new SimpleDrawComponent(this, w, h, tc);
    game->drawables.emplace_back(sdc);
    cc=new CollisionComponent(this, tc, sdc);

    AddComponent(tc);
    AddComponent(sdc);
    AddComponent(cc);
}

void Ball::UpdateActor(float deltaTime){
    if(cc->tc->position.x < -cc->sdc->size.x){
        game->mIsRunning=false;
    }
}
