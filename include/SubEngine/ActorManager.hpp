#pragma once

#include <optional>
#include <vector>

#include "Ownership.hpp"
#include "SubEngine/SubEngine.hpp"

class ActorManager: public SubEngine,
    public Owner<Actor>
{
  public:
    virtual ~ActorManager()=default;

    std::optional<std::shared_ptr<ISubEngine>>
    query(const SubEngineName name) noexcept;

  protected:
    ActorManager() noexcept=default;

  private:
    void appendActor(const std::shared_ptr<IActor> actor) noexcept;
    void removeActor(const std::shared_ptr<IActor> actor) noexcept;

  private:
    SubEngineName getName() const noexcept override final{
        return SubEngineName::ActorManager;
    }
    void update(const float &deltaTime) noexcept override final;
    void injectDependency() noexcept override final{ setAttribute(); }

    void onNotify(Lifetime lifetime, std::shared_ptr<Actor> actor) noexcept override final;

  private:
    virtual void setAttribute() noexcept=0;

  private:
    bool isUpdatingActors = false;
    std::vector<std::shared_ptr<IActor>> actors;
    std::vector<std::shared_ptr<IActor>> pendingActors;
};

class NullActorManager final: public ActorManager{
private:
    void setAttribute() noexcept override final{}
};
