#pragma once

#include "System/Logging/Bind.hpp"

class Resources{
  public:
    virtual ~Resources()=default;

  private:
    Logging::Bind logger={"Resources", "base"};
};
