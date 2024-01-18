#pragma once

#include <optional>
#include <vector>

#include "Observer.hpp"
#include "SubEngine/SubEngine.hpp"

class ActorManager: public SubEngine,
    public Observer<Lifetime, IActor>
{
  public:
    virtual ~ActorManager()=default;

    std::optional<std::shared_ptr<ISubEngine>>
    query(const SubEngineName name) noexcept;


  protected:
    ActorManager() noexcept=default;

  private:
    SubEngineName getName() const noexcept override final{
        return SubEngineName::ActorManager;
    }
    void update(const float &deltaTime) noexcept override final;

    void onNotify(MSG_t lifetime, spObservable actor) noexcept override final;
    void appendActor(const std::shared_ptr<IActor> actor) noexcept;
    void removeActor(const std::shared_ptr<IActor> actor) noexcept;

  private:
    virtual void injectDependency() noexcept override=0;

  private:
    bool isUpdatingActors = false;
    std::vector<std::shared_ptr<IActor>> actors;
    std::vector<std::shared_ptr<IActor>> pendingActors;
};

class NullActorManager: public ActorManager{
private:
    void injectDependency() noexcept override final{}
};
