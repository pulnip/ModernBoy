#include <algorithm>
#include <cassert>

#include "GameEngine/GameEngine.hpp"
#include "SubEngine/ActorManager.hpp"
#include "Actor/Actor.hpp"

using namespace Game;
using namespace Game::SubEngine;

void ActorManager::postConstruct() noexcept{
    assert(not owner.expired());
    owner.lock()->setActorManager(shared_from_this());
}

std::optional<Plugin> ActorManager::query(
    const SubEngine::Type name
) noexcept{
    auto result=owner.lock()->find(name);

    if(result == nullptr){
        return std::nullopt;
    }
    return result;
}

void ActorManager::appendActor(const Object actor) noexcept {
    if (isUpdatingActors) {
        pendingActors.emplace_back(actor);
    } else {
        actors.emplace_back(actor);
    }
}

void ActorManager::removeActor(const Object actor) noexcept {
    const auto it=actors.erase(
        std::remove(actors.begin(), actors.end(), actor)
    );

    // actor not found in actors
    if (it == actors.end()) {
        pendingActors.erase(
            std::remove(actors.begin(), actors.end(), actor)
        );
    }
}

void ActorManager::update(const Time& deltaTime) noexcept {
    isUpdatingActors = true;
    // 모든 액터를 갱신
    for (auto& actor : actors) {
        actor->update(deltaTime);
    }
    isUpdatingActors = false;

    // 대기 중인 액터를 활성화
    for (auto& actor : pendingActors) {
        actors.emplace_back(actor);
    }
    pendingActors.clear();

    // 죽은 액터를 제거
    actors.erase(
        std::remove_if(actors.begin(), actors.end(),
            [](const auto& actor) {
                return actor->getState()==Actor::State::EDead;
            }
        ), actors.end()
    );
}
