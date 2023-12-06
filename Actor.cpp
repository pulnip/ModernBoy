#include <algorithm>

#include "Actor.hpp"
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

Component::Component(Actor* owner, int updateOrder)
:mOwner(owner), mUpdateOrder(updateOrder){}
Component::~Component(){}

void Component::update(float deltaTime){}
