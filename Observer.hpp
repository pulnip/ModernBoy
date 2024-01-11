#pragma once

#include "my_concepts.hpp"

template <typename MSG, typename T>
    requires(!deref_able<MSG>)
class Observable;

template <typename MSG, typename T = void>
    requires(!deref_able<MSG>)
class Observer {
    friend class Observable<MSG, T>;

  public:
    virtual ~Observer() = default;

  private:
    virtual void onNotify(MSG msg, T t) = 0;
};

template <typename MSG>
    requires(!deref_able<MSG>)
class Observer<MSG, void> {
    friend class Observable<MSG, void>;

  public:
    virtual ~Observer() = default;

  private:
    virtual void onNotify(MSG msg) = 0;
};
