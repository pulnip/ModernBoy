#pragma once

#include <memory>
#include <vector>
#include "System/Logging/Bind.hpp"
#include "Engine/Types.hpp"
#include "FWD.hpp"

class Actors{
    using pActor=std::shared_ptr<Actor>;
  public:
    Actors() noexcept=default;
    virtual ~Actors()=default;

    void update(const Game::Time& deltaTime) noexcept;

    void append(pActor actor) noexcept;
    void remove(int id) noexcept;

  public:
    static std::shared_ptr<Actors> null;

  private:
    Logging::Bind logger={"Actors", "base"};
    bool isUpdating=false;
    std::vector<pActor> actors;
    std::vector<pActor> pendingActors;
};

class NullActors: public Actors{};
