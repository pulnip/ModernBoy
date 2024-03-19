#include <algorithm>
#include "Engine/Core.hpp"
#include "Engine/PhysicsSimulator.hpp"
#include "Actor/Vanilla.hpp"
#include "Component/Movable.hpp"

using namespace Engine;

template<typename T>
bool isEqual(
    const std::weak_ptr<T>& lhs, const std::weak_ptr<T>& rhs
) noexcept{
    return not (lhs.owner_before(rhs) or rhs.owner_before(lhs));
}

void PhysicsSimulator::update(const Game::Time& deltaTime) noexcept{
    // remove if expired
    std::erase_if(collisionMap, [](const auto& pair){
        return pair.first.expired();
    });
    for(auto& [_, li]: collisionMap){
        li.remove_if([](const auto& wp){
            return wp.expired();
        });
    }

    // check collision
    // for(auto& [refTarget, list]: collisionMap){
    //     assert(not refTarget.expired());
    //     auto target=refTarget.lock();
    //     // update movable
    //     target->get().update(deltaTime);

    //     for(auto& refOppo: list){
    //         assert(not refOppo.expired());
    //         auto opponent=refOppo.lock();

    //         redoUpdateIfCollide(target, opponent, deltaTime);
    //     }
    // }

    for(auto& refPlayer: players){
        assert(not refPlayer.expired());
        auto player=refPlayer.lock();

        player->get().update(deltaTime);

        for(auto& refOther: players){
            if(isEqual(refPlayer, refOther)) break;

            assert(not refOther.expired());
            auto other=refOther.lock();

            redoUpdateIfCollide(player, other, deltaTime);
        }

        for(auto& refNeutural: neutrals){
            assert(not refNeutural.expired());
            auto neutral=refNeutural.lock();

            redoUpdateIfCollide(player, neutral, deltaTime);
        }
    }

    for(auto& refEnemy: enemys){
        assert(not refEnemy.expired());
        auto enemy=refEnemy.lock();

        enemy->get().update(deltaTime);

        for(auto& refOther: enemys){
            if(isEqual(refEnemy, refOther)) break;

            assert(not refOther.expired());
            auto other=refOther.lock();

            redoUpdateIfCollide(enemy, other, deltaTime);
        }

        for(auto& refNeutural: neutrals){
            assert(not refNeutural.expired());
            auto neutral=refNeutural.lock();

            redoUpdateIfCollide(enemy, neutral, deltaTime);
        }
        for(auto& refPlayer: players){
            assert(not refPlayer.expired());
            auto player=refPlayer.lock();

            redoUpdateIfCollide(enemy, player, deltaTime);
        }
    }
}

void PhysicsSimulator::append(pActor actor) noexcept{
    auto opt=actor->get(Component::Type::Movable);

    if(opt.has_value()){
        auto movable=std::static_pointer_cast<Component::Movable>(opt.value());
        collisionMap.try_emplace(movable, std::list<wp>());

        switch(actor->getRole()){
          case Actor::Role::Neutral:
            neutrals.emplace_back(movable);
            break;
          case Actor::Role::Player:
            players.emplace_back(movable);
            break;
          case Actor::Role::Enemy:
            enemys.emplace_back(movable);
        }
    } else{
        logger.info("Movable not found");
    }
}

template<typename T>
bool is_same(std::weak_ptr<T> lhs, std::weak_ptr<T> rhs){
    return (not lhs.owner_before(rhs)) && (not rhs.owner_before(lhs));
}

void PhysicsSimulator::setCollision(pActor who, pActor to) noexcept{
    using namespace Component;

    auto optWho=who->get(Type::Movable);
    auto optTo=to->get(Type::Movable);

    if(not optWho.has_value()){
        logger.info("who not movable");
        return;
    }
    if(not optTo.has_value()){
        logger.info("to not movable");
        return;
    }

    auto mcWho=std::static_pointer_cast<Movable>(optWho.value());
    auto mcTo=std::static_pointer_cast<Movable>(optTo.value());

    auto it=collisionMap.lower_bound(mcWho);

    if( it == collisionMap.end() or
        not is_same<Movable>(mcWho, it->first)
    ){
        // key not exist
        collisionMap.emplace_hint(it,
            mcWho, std::list<wp>{mcTo}
        );
    } else{
        // key already exists
        it->second.emplace_back(mcTo);
    }
}
