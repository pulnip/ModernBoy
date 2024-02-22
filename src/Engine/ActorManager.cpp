#include <algorithm>
#include "Engine/ActorManager.hpp"
#include "Engine/Logger.hpp"
#include "Actor/Vanilla.hpp"

using namespace Engine;

ActorManager::ActorManager() noexcept:
    logger(std::make_unique<BindedLogger>("ActorManager", "Base"))
{
    logger->debug("constructed");
}

ActorManager::~ActorManager(){
    logger->debug("destructed");
}

void ActorManager::appendActor(pActor actor) noexcept{
    logger->debug("append actor");

    if (isUpdatingActors) {
        pendingActors.emplace_back(actor);
    } else {
        actors.emplace_back(actor);
    }
}

void ActorManager::removeActor(pActor actor) noexcept {
    logger->debug("remove actor");

    const auto it=actors.erase(
        std::remove(actors.begin(), actors.end(), actor)
    );

    // actor not found in actors
    if(it == actors.end()) {
        pendingActors.erase(
            std::remove(actors.begin(), actors.end(), actor)
        );
    }
}

void ActorManager::update(const Game::Time& deltaTime) noexcept{
    logger->debug("update");

    isUpdatingActors = true;
    // 모든 액터를 갱신
    for (auto& actor: actors) {
        actor->update(deltaTime);
    }
    isUpdatingActors = false;

    // 대기 중인 액터를 활성화
    for(auto& actor: pendingActors) {
        actors.emplace_back(actor);
    }
    pendingActors.clear();

    // 죽은 액터를 제거
    actors.erase(
        std::remove_if(actors.begin(), actors.end(),
            [](const auto& actor) {
                return actor->getState()==Actor::State::Dead;
            }
        ), actors.end()
    );
}
