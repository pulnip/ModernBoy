#include "physics.hpp"
#include "game/context.hpp"
#include "game/physics_system.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

PhysicsSystem::PhysicsSystem(EntityRegistry& registry)
:registry(registry){}

void PhysicsSystem::update(DeltaTime dt){
    auto dt_ = dt.count() / 1'000'000.0f;

    auto vec = asVector();
    std::vector<bool> collided(vec.size(), false);

    for(Index i=0; i<vec.size(); ++i){
        auto [src_id, src_bit, src_tc, src_rc, src_sc] = vec[i];
        auto src_pos = src_tc->position + src_sc->position;
        auto src_rad = src_sc->radius;

        for(Index j=i+1; j<vec.size(); ++j){
            auto [tgt_id, tgt_bit, tgt_tc, tgt_rc, tgt_sc] = vec[j];
            auto tgt_pos = tgt_tc->position + tgt_sc->position;
            auto tgt_rad = tgt_sc->radius;

            if(sphereCollision(src_pos, src_rad, tgt_pos, tgt_rad)){
                collided[i] = true;
                collided[j] = true;
            }
        }
    }

    for(Index i=0; i<vec.size(); ++i){
        auto [id, bit, _1, _2, _3] = vec[i];
        auto [_4, collided_before] = registry.query_safe<Collided>(id);
        bool collided_now = collided[i];

        if(collided_now && !collided_before){
            registry.appendComponent(id, Collided{.entity=id, .isActive=true});
            auto [mc, has_mc] = registry.query_safe<Model>(id);
            if(has_mc)
                mc.alpha = 0.5f;
        }
        else if(!collided_now && collided_before){
            registry.removeComponent<Collided>(id);
            auto [mc, has_mc] = registry.query_safe<Model>(id);
            if(has_mc)
                mc.alpha = 1.0f;
        }
    }

    for(auto [id, bit, tc, rc]: registry.query<Transform, Rigidbody>()){
        tc.position += rc.velocity * dt_;
    }
}

std::vector<SphereColliderProxy> PhysicsSystem::asVector(){
    std::vector<SphereColliderProxy> flat;

    for(auto [id, bit, tc, rc, sc]: registry.query<Transform, Rigidbody, SphereCollider>()){
        flat.emplace_back(id, bit, &tc, &rc, &sc);
    }

    return flat;
}
