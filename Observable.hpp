#pragma once

#include <list>
#include <memory>
#include <ranges>

#include "Observer.hpp"

enum class Observable_msg {
    CONSTRUCTED,
    DESTRUCTED
};

template <typename T>
class Observable {
  public:
    virtual ~Observable() = default;

    void subscribe(std::weak_ptr<Observer<T>> o) {
        observers.emplace_back(std::move(o));
    }
    void unsubscribe(std::weak_ptr<Observer<T>> o) {
        observers.remove(o);
    }

  protected:
    void notify(std::shared_ptr<T> self, Observable_msg msg) {
        std::list<std::weak_ptr<Observer<T>>> expired_observers;

        for (auto &o : observers) {
            if (!o.expired()) {
                o.lock()->onNotify(self, msg);
            } else {
                expired_observers.emplace_back(o);
            }
        }

        for (auto &eo : expired_observers) {
            observers.remove(eo);
        }
    }

  private:
    std::list<std::weak_ptr<Observer<T>>> observers;
};
