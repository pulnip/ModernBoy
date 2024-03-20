#pragma once

#include "System/Logging/Bind.hpp"

class Sounds{
  public:
    virtual ~Sounds()=default;

  private:
    Logging::Bind logger={"Sounds", "base"};
};
