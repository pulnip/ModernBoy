#pragma once

#include "System/Logging/Bind.hpp"
#include "System/Time/Types.hpp"

class Timer{
  public:
    using unit = Time::millisecond;

    virtual ~Timer()=default;

    virtual void reset() noexcept=0;
    virtual void wait(const unit timeout) noexcept=0;
    virtual unit getDeltaTime() noexcept=0;

  private:
    Logging::Bind logger={"Timer", "base"};
};