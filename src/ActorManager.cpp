#include <algorithm>
#include <cassert>

#include "Actor/Actor.hpp"
#include "PubSubMessage.hpp"
#include "SubEngine/ActorManager.hpp"

std::optional<std::weak_ptr<SubEngine>>
ActorManager::requestSubEngine(const SubEngineName name) noexcept {
#warning "Not Defined"
    return std::nullopt;
}

void ActorManager::update(const float& deltaTime) noexcept {
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
    actors.erase(std::remove_if(actors.begin(), actors.end(),
                                [](const auto& actor) {
                                    return actor->getState() ==
                                           Actor::State::EDead;
                                }),
                 actors.end());
}

void ActorManager::postConstruct() noexcept {
#warning "Not defined"
}

void ActorManager::onNotify(MSG_t lifetime, spObservable actor) noexcept {
    switch (lifetime) {
    case Lifetime::CONSTRUCTED:
        appendActor(actor);
        break;
    case Lifetime::DESTRUCTED:
        removeActor(actor);
        break;
    default:
        assert(false);
    }
}

void ActorManager::appendActor(const std::shared_ptr<IActor> actor) noexcept {
    if (isUpdatingActors) {
        pendingActors.emplace_back(actor);
    } else {
        actors.emplace_back(actor);
    }
}

void ActorManager::removeActor(const std::shared_ptr<IActor> actor) noexcept {
    const auto it =
        actors.erase(std::remove(actors.begin(), actors.end(), actor));

    // actor not found in actors
    if (it == actors.end()) {
        pendingActors.erase(std::remove(actors.begin(), actors.end(), actor));
    }
}