#pragma once

#include <memory>
#include "FWD.hpp"
#include "Engine/Types.hpp"
#include "System/Logging/Bind.hpp"
#include "Component/Traits.hpp"

class Component{
  protected:
    using Type=ComponentTraits::Type;
  public:
    Component(int updateOrder) noexcept:
        updateOrder(updateOrder){}
    virtual ~Component()=default;

    virtual void update(const Game::Time&, Actor&)=0;

    virtual Type getType() noexcept{
        return Type::Base;
    }
    int getUpdateOrder() const noexcept{
        return updateOrder;
    }

  protected:
    const int id = seed++;

  private:
    static int seed;
    int updateOrder;
    Logging::Bind logger={"Component", "base"};
};

class NullComponent final: public Component{
    void update(const Game::Time&, Actor&) override{ }
  private:
    Logging::Bind logger={"Component", "Null"};
};
