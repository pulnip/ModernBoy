#include <algorithm>
#include "System/Managing/Actors.hpp"
#include "Actor/Actor.hpp"
#include "Actor/Traits.hpp"

void Actors::append(pActor actor) noexcept{
    if (isUpdating) {
        pendingActors.emplace(actor->id, actor);
    } else {
        actors.emplace(actor->id, actor);
    }
}

void Actors::remove(int id) noexcept {
    logger.debug("remove actor");

    actors.erase(id);
    pendingActors.erase(id);
}

void Actors::update(const Game::Time& dt) noexcept{
    isUpdating = true;
    // 모든 액터를 갱신
    for (auto& [_, actor]: actors) {
        actor->update(dt);
    }
    isUpdating = false;

    // 죽은 액터를 제거
    std::erase_if(actors,
        [](const auto& pair){
            return pair.second->getState() == ActorTraits::State::Dead;
        }
    );

    // 대기 중인 액터를 활성화
    for(auto& [id, actor]: pendingActors) {
        actors.emplace(id, actor);
    }
    pendingActors.clear();

}
