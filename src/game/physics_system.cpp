#include "physics.hpp"
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

    auto collisionPair = findCandidate();

    for(auto [src, tgt]: collisionPair){
        auto [src_tc, src_rc, src_sc] = src;
        auto [tgt_tc, tgt_rc, tgt_sc] = tgt;

        auto src_pos = src_tc->position + tgt_tc->position;
        auto tgt_pos = tgt_tc->position + tgt_tc->position;

        if(sphereCollision(src_pos, src_sc->radius, tgt_pos, tgt_sc->radius)){
            std::println("collision!");
        }
        else{
            std::println("Not collision!");
        }
    }

    co_return;
}

std::vector<SphereColliderProxy> PhysicsSystem::flatten(){
    std::vector<SphereColliderProxy> flat;

    for(auto [tc, rc, sc]: registry.query<TransformComponent, RigidbodyComponent, SphereColliderComponent>()){
        flat.emplace_back(&tc, &rc, &sc);
    }

    return flat;
}
std::vector<SphereCollisionCandidate> PhysicsSystem::findCandidate(){
    std::vector<SphereColliderProxy> proxies = flatten();


    std::vector<SphereCollisionCandidate> candidates;
    for(Index i=0; i<proxies.size(); ++i){
        auto [src_tc, src_rc, src_sc] = proxies[i];

        for(Index j=i+1; j<proxies.size(); ++j){
            auto [tgt_tc, tgt_rc, tgt_sc] = proxies[j];

            auto tgt_to_src = src_tc->position - tgt_tc->position;
            auto rel_vel = tgt_rc->velocity - src_rc->velocity;

            if(dot(tgt_to_src, rel_vel) > 0)
                candidates.emplace_back(proxies[i], proxies[j]);
        }
    }

    return candidates;
}