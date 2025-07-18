#include "game/context.hpp"
#include "game/physics_system.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

PhysicsSystem::PhysicsSystem(EntityRegistry& registry)
:registry(registry){}

Generator<void> PhysicsSystem::update(DeltaTime dt){
    auto dt_ = dt.count() / 1'000'000.0f;

    for(auto [tc, rc]: registry.query<TransformComponent, RigidbodyComponent>()){
        tc.position += rc.velocity * dt_;

        co_yield 0;
    }

    co_return;
}
