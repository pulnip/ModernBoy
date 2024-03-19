#pragma once

#include <memory>

class Callable{
  public:
    virtual void operator()() noexcept=0;
};

class DoNothing final: public Callable{
  public:
    void operator()() noexcept override{}
};

extern std::shared_ptr<DoNothing> doNothing;

class Predicate{
  public:
    virtual bool operator()() noexcept=0;
};

class AlwaysTrue: public Predicate{
  public:
    bool operator()() noexcept override{ return true; }
};

extern std::shared_ptr<AlwaysTrue> alwaysTrue;