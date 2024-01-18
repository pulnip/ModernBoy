#pragma once

#include <memory>
#include <optional>

#include "Observer.hpp"
#include "gefwd.hpp"

class ISubEngine{
  public:
    virtual ~ISubEngine()=default;

    virtual void update(const float& deltaTime) noexcept=0;
    virtual SubEngineName getName() const noexcept=0;

    virtual std::optional<std::shared_ptr<ISubEngine>>
    query(const SubEngineName name) noexcept=0;
};