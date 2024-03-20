#pragma once

#include <memory>
#include "FWD.hpp"

class Main{
  public:
    Main() noexcept;

    void start() noexcept;

    std::shared_ptr<Actors> actors;
    std::shared_ptr<Logger> logger;
};