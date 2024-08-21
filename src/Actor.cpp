#include <vector>
#include "Actor.hpp"
#include "Attributes.hpp"
#include "Core.hpp"

using namespace std;
using namespace ModernBoy;

Actor::Actor([[maybe_unused]]const Core& c){

}

Actor::~Actor()=default;

void Actor::update([[maybe_unused]] float dt){

}

void Actor::addComponent(shared_ptr<Component> comp){
    components.emplace_back(comp);
}
