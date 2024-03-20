#pragma once

#include <memory>
#include <string>
#include "FWD.hpp"
#include "System/Logging/Bind.hpp"
#include "Engine/Types.hpp"
#include "Component/Component.hpp"
#include "Component/Movable/Types.hpp"

class Movable: public Component{
    using Transform2D=Transform::Transform2D;
  public:
    Movable() noexcept=default;
    Movable(const Transform2D&) noexcept;

    void update(const Game::Time&, Actor&) noexcept override;
    Type getType() noexcept{
        return Type::Movable;
    }
    auto& get(){ return transform; }

  private:
    Logging::Bind logger={"Movable", std::to_string(id)};
    Transform2D transform;
};
