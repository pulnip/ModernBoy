#include <print>
#include <vector>
#include "Actor.hpp"
#include "ActorManager.hpp"

using namespace std;
using namespace ModernBoy;

ActorManager::ActorManager(){

}

void ActorManager::update([[maybe_unused]] float dt){

}

void ActorManager::addActor(shared_ptr<Actor> actor){
    actors.emplace_back(actor);
}
