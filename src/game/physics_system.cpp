#include "engine/physics/physics.hpp"
#include "context.hpp"
#include "physics_system.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

PhysicsSystem::PhysicsSystem(EntityRegistry& registry)
:registry(registry){}

void PhysicsSystem::update(DeltaTime dt){
    auto dt_ = dt.count() / 1'000'000.0f;

    sphere2sphereCollision();
    box2boxCollision();

    simulateGravity(dt);

    for(auto [id, bit, tc, rc]: registry.query<Transform, Rigidbody>()){
        tc.position += rc.velocity * dt_;
    }
}

void PhysicsSystem::simulateGravity(DeltaTime dt){
    auto dt_ = dt.count() / 1'000'000.0f;

    for(auto [id, bit, tf, rb]: registry.query<Transform, Rigidbody>()){
        if(bit & GROUNDED_BIT)
            continue;
        if(!rb.useGravity)
            continue;
        rb.velocity += 0.5*dt_ * Vec3{.x=0, .y=-1, .z=0};
    }
}

void PhysicsSystem::sphere2sphereCollision(){
    // sphere to sphere collision
    auto vec = getSphereColliderProxies();
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
}

void PhysicsSystem::box2boxCollision(){
    // box to box collision
    auto vec = getBoxColliderProxies();
    std::vector<bool> collided(vec.size(), false);

    for(Index i=0; i<vec.size(); ++i){
        auto [src_id, src_bit, src_tf, src_rb, src_bc] = vec[i];
        auto src_pos = src_tf->position + src_bc->position;
        auto src_rot = src_tf->rotation * src_bc->rotation;
        auto src_scl = src_tf->scale * src_bc->scale;

        auto src_axis_x = src_scl.x * right(src_rot);
        auto src_axis_y = src_scl.y * up(src_rot);
        auto src_axis_z = src_scl.z * forward(src_rot);

        for(Index j=i+1; j<vec.size(); ++j){
            auto [tgt_id, tgt_bit, tgt_tf, tgt_rb, tgt_bc] = vec[i];
            auto tgt_pos = tgt_tf->position + tgt_bc->position;
            auto tgt_rot = tgt_tf->rotation * tgt_bc->rotation;
            auto tgt_scl = tgt_tf->scale * tgt_bc->scale;

            auto tgt_axis_x = tgt_scl.x * right(tgt_rot);
            auto tgt_axis_y = tgt_scl.y * up(tgt_rot);
            auto tgt_axis_z = tgt_scl.z * forward(tgt_rot);

            if(obb(src_pos, src_axis_x, src_axis_y, src_axis_z,
                tgt_pos, tgt_axis_x, tgt_axis_y, tgt_axis_z)
            ){
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
                mc.alpha = 0.0f;
        }
        else if(!collided_now && collided_before){
            registry.removeComponent<Collided>(id);
            auto [mc, has_mc] = registry.query_safe<Model>(id);
            if(has_mc)
                mc.alpha = 1.0f;
        }
    }
}

std::vector<SphereColliderProxy> PhysicsSystem::getSphereColliderProxies(){
    std::vector<SphereColliderProxy> flat;
    for(auto [id, bit, tc, rc, sc]: registry.query<Transform, Rigidbody, SphereCollider>())
        flat.emplace_back(id, bit, &tc, &rc, &sc);
    return flat;
}

std::vector<BoxColliderProxy> PhysicsSystem::getBoxColliderProxies(){
    std::vector<BoxColliderProxy> flat;
    for(auto [id, bit, tf, rb, bc]: registry.query<Transform, Rigidbody, BoxCollider>())
        flat.emplace_back(id, bit, &tf, &rb, &bc);
    return flat;
}
