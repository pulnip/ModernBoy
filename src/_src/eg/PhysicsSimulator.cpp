#include <algorithm>
#include "Engine/Core.hpp"
#include "Engine/PhysicsSimulator.hpp"
#include "Actor/Vanilla.hpp"
#include "Component/Movable.hpp"

using namespace Engine;

PhysicsSimulator::PhysicsSimulator() noexcept{
    using namespace Actor;

    targets.emplace(Role::Neutral, std::vector<wp>());
    targets.emplace(Role::Player, std::vector<wp>());
    targets.emplace(Role::Enemy, std::vector<wp>());
}

template<typename T>
bool isEqual(std::weak_ptr<T> lhs, std::weak_ptr<T> rhs){
    return (not lhs.owner_before(rhs)) && (not rhs.owner_before(lhs));
}

void PhysicsSimulator::update(const Game::Time& deltaTime) noexcept{
    // remove if expired
    for(auto& [_, vector]: targets){
        std::erase_if(vector, [](const auto& ptr){
            return ptr.expired();
        });
    }

    for(auto& [role, vector]: targets){
        for(auto& target: vector){
            updateTarget(target, role, deltaTime);
        }
    }
}

bool PhysicsSimulator::updateTarget(
    wp target, const Actor::Role& role, const Game::Time& dt
) noexcept{
    using Actor::Role;
    if(target.expired()) return false;

    auto movable=target.lock();
    movable->get().update(dt);

    switch(role){
      case Role::Enemy:
        updateTarget_aux(movable, targets[Role::Enemy], dt);
        [[fallthrough]];
      case Role::Player:
        updateTarget_aux(movable, targets[Role::Player], dt);
        updateTarget_aux(movable, targets[Role::Neutral], dt);
    }

    return true;
}

void PhysicsSimulator::updateTarget_aux(
    sp movable, const std::vector<wp>& ops, const Game::Time& dt
) noexcept{
    for(auto& ref: ops){
        if(ref.expired()) break;
        if(isEqual<Component::Movable>(movable, ref)) break;

        redoUpdateIfCollide(movable, ref.lock(), dt);
    }
}

void PhysicsSimulator::append(pActor actor) noexcept{
    auto opt=actor->get(Component::Type::Movable);

    if(opt.has_value()){
        auto movable=std::static_pointer_cast<Component::Movable>(opt.value());

        targets[actor->getRole()].emplace_back(movable);
    } else{
        logger.info("Movable not found");
    }
}
