#pragma once

#include <algorithm>
#include <list>
#include <memory>

#include "Observer.hpp"

template <typename MSG, typename type_hint>
class Observable_impl {
  private:
    using ObserverRef = std::weak_ptr<Observer<MSG, type_hint>>;

  public:
    virtual ~Observable_impl() = default;

    void subscribe(ObserverRef o) {
        observers.emplace_back(std::move(o));
    }
    void unsubscribe(ObserverRef o) {
        observers.remove(std::move(o));
    }

  protected:
    void notify(MSG msg) {
        remove_if_expired();

        for (auto &wo : observers) {
            notify_aux(msg, wo);
        }
    }

  protected:
    virtual void notify_aux(MSG msg, ObserverRef target) = 0;

    void remove_if_expired() {
        std::remove_if(observers.begin(), observers.end(), [](const auto &wp) {
            return wp.expired();
        });
    }

  private:
    std::list<ObserverRef> observers;
};

template <typename MSG, typename type_hint>
class Observable : public Observable_impl<MSG, type_hint>, public std::enable_shared_from_this<type_hint> {
  public:
    virtual ~Observable() = default;

  private:
    void notify_aux(MSG msg, std::weak_ptr<Observer<MSG, type_hint>> target) override {
        if (target.expired())
            return;
        target.lock()->onNotify(msg, this->shared_from_this());
    }
};

template <typename MSG>
class Observable<MSG, void> : public Observable_impl<MSG, void> {
  public:
    virtual ~Observable() = default;

  private:
    void notify_aux(MSG msg, std::weak_ptr<Observer<MSG, void>> target) override {
        if (target.expired())
            return;

        target.lock()->onNotify(msg);
    }
};
