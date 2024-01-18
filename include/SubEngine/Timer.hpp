#pragma once

#include <cstdint>
#include "Makable.hpp"

class Timer : public Makable<Timer> {
  public:
    using millisecond = uint64_t;

  public:
    Timer() = default;
    virtual ~Timer() = default;

    virtual void reset() noexcept = 0;
    virtual void wait(const millisecond timeout) noexcept = 0;
    virtual millisecond getDeltaTime() noexcept = 0;

  private:
    void postConstruct() noexcept override {}
};