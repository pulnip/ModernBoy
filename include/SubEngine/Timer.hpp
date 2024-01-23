#pragma once

#include <cstdint>
#include "Makable.hpp"

class Timer:
    public Makable<Timer>
{
  public:
    using millisecond = uint64_t;
  public:
    virtual ~Timer() = default;

  protected:
    Timer() = default;

  private:
    void postConstruct() noexcept override final{ injectDependency(); }

  public:
    virtual void injectDependency() noexcept=0;
    virtual void reset() noexcept=0;
    virtual void wait(const millisecond timeout) noexcept=0;
    virtual millisecond getDeltaTime() noexcept=0;
};