#include "game/physics_system.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

PhysicsSystem::PhysicsSystem(Context& world)
:world(world){}

size_t PhysicsSystem::yield_count() const noexcept{

}

Generator<void> PhysicsSystem::updateTask(DeltaTime dt){

}

Generator<void> PhysicsSystem::update(DeltaTime dt){

}
