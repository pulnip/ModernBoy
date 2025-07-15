#include "game/context.hpp"
#include "game/physics_system.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

PhysicsSystem::PhysicsSystem(Context& world)
:world(world){}

Generator<void> PhysicsSystem::update(DeltaTime dt){
    auto dt_ = dt.count() / 1'000'000.0f;

    for(auto& [bit, vec]: world.query<TransformComponent, RigidbodyComponent>()){
        vec.transform([bit, dt_](void* chunk){
            auto& tc = *static_cast<TransformComponent*>(Util::add(chunk, offset_of<TransformComponent>(bit)));
            auto& rc = *static_cast<RigidbodyComponent*>(Util::add(chunk, offset_of<RigidbodyComponent>(bit)));

            tc.value.position += rc.velocity * dt_;
        });
        co_yield 0;
    }

    co_return;
}
