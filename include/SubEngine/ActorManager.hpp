#pragma once

#include <optional>
#include <vector>

#include "Observer.hpp"
#include "PubSubMessage.hpp"

#include "SubEngine/SubEngine.hpp"

#include "gefwd.hpp"

class ActorManager : public SubEngine, public Observer<Lifetime, IActor> {
  public:
    std::optional<std::weak_ptr<SubEngine>>
    requestSubEngine(const SubEngineName name) noexcept;
    void update(const float &deltaTime) noexcept override;

  protected:
    ActorManager() noexcept=default;

  private:
    void postConstruct() noexcept override;
    void onNotify(MSG_t lifetime, spObservable actor) noexcept override;

    void appendActor(const std::shared_ptr<IActor> actor) noexcept;
    void removeActor(const std::shared_ptr<IActor> actor) noexcept;

  private:
    bool isUpdatingActors = false;
    std::vector<std::shared_ptr<IActor>> actors;
    std::vector<std::shared_ptr<IActor>> pendingActors;
};
