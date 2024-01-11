#pragma once

#include <list>
#include <memory>

#include "my_concepts.hpp"

#include "Observer.hpp"

template <typename MSG, typename T = void>
    requires(!deref_able<MSG>)
class Observable {
  public:
    virtual ~Observable() = default;

    void subscribe(std::weak_ptr<Observer<MSG, T>> o) {
        observers.emplace_back(std::move(o));
    }
    void unsubscribe(std::weak_ptr<Observer<MSG, T>> o) {
        observers.remove(o);
    }

  protected:
    void notify(MSG msg, T t) {
        decltype(observers) expired_observers;

        for (auto &o : observers) {
            if (!o.expired()) {
                o.lock()->onNotify(msg, t);
            } else {
                expired_observers.emplace_back(o);
            }
        }

        for (auto &eo : expired_observers) {
            observers.remove(eo);
        }
    }

  private:
    std::list<std::weak_ptr<Observer<MSG, T>>> observers;
};

template <typename MSG>
    requires(!deref_able<MSG>)
class Observable<MSG, void> {
  public:
    virtual ~Observable() = default;

    void subscribe(std::weak_ptr<Observer<MSG>> o) {
        observers.emplace_back(std::move(o));
    }
    void unsubscribe(std::weak_ptr<Observer<MSG>> o) {
        observers.remove(o);
    }

  protected:
    void notify(MSG msg) {
        decltype(observers) expired_observers;

        for (auto &o : observers) {
            if (!o.expired()) {
                o.lock()->onNotify(msg);
            } else {
                expired_observers.emplace_back(o);
            }
        }

        for (auto &eo : expired_observers) {
            observers.remove(eo);
        }
    }

  private:
    std::list<std::weak_ptr<Observer<MSG>>> observers;
};
