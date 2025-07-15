#include "game/context.hpp"
#include "game/physics_system.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

PhysicsSystem::PhysicsSystem(Context& world)
:world(world){}

Generator<void> PhysicsSystem::update(DeltaTime dt){
    auto dt_ = dt.count() / 1'000'000.0f;

    for(auto& [bit, vec]: world.query<TransformComponent, RigidbodyComponent>()){
        vec.transform([dt_](TransformComponent& tc, RigidbodyComponent& rc){
            tc.value.position += rc.velocity * dt_;
        });
    }
}
