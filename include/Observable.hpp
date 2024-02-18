#pragma once

#include <algorithm>
#include <cassert>
#include <list>

#include "Observer.hpp"

template <typename MSG, typename type_hint>
class Observable_impl{
  public:
    Observable_impl() noexcept=default;
    virtual ~Observable_impl()=default;

    void subscribe(std::weak_ptr<Observer<MSG, type_hint>> o) noexcept{
        observers.emplace_back(std::move(o));
    }
    void unsubscribe(std::weak_ptr<Observer<MSG, type_hint>> o) noexcept{
        observers.remove_if([&o](const auto& p){
            return not (p.owner_before(o) || o.owner_before(p));
        });
    }

    void notify(const MSG& msg) noexcept{
        remove_if_expired();

        for (auto &wo : observers) {
            notify_aux(msg, wo);
        }
    }

  private:
    void remove_if_expired() noexcept{
        std::remove_if(
            observers.begin(), observers.end(),
            [](const auto &wp) {
                return wp.expired();
            }
        );
    }

  private:
    virtual void notify_aux(const MSG& msg, std::weak_ptr<Observer<MSG, type_hint>> target) noexcept=0;

  private:
    std::list<std::weak_ptr<Observer<MSG, type_hint>>> observers;
};

template <typename MSG, typename type_hint>
class Observable: public Observable_impl<MSG, type_hint>,
    public std::enable_shared_from_this<type_hint>
{
  public:
    Observable() noexcept=default;
    virtual ~Observable()=default;

  private:
    void notify_aux(
        const MSG& msg,
        std::weak_ptr<Observer<MSG, type_hint>> target
    ) noexcept override final{
        static_assert(std::derived_from<
            type_hint, Observable<MSG, type_hint>
        >);
        assert(!target.expired());
        target.lock()->onNotify(msg, this->shared_from_this());
    }
};

template <typename MSG>
class Observable<MSG, void>: public Observable_impl<MSG, void>{
  public:
    Observable() noexcept=default;
    virtual ~Observable()=default;

  private:
    void notify_aux(const MSG& msg, std::weak_ptr<Observer<MSG>> target) noexcept override final{
        assert(!target.expired());
        target.lock()->onNotify(msg);
    }
};

// observable for one subscriber

template <typename MSG, typename type_hint>
class UniqueObservable_impl{
  public:
    UniqueObservable_impl() noexcept=default;
    virtual ~UniqueObservable_impl()=default;

    void subscribe(std::weak_ptr<Observer<MSG, type_hint>> o) noexcept{
        observer=o;
    }
    void unsubscribe() noexcept{
        observer.reset();
    }

    void notify(const MSG& msg) noexcept{
        if(observer.expired()){
            observer.reset();
            return;
        }

        notify_aux(msg, observer);
    }

  private:
    virtual void notify_aux(
        const MSG& msg,
        std::weak_ptr<Observer<MSG, type_hint>> target
    ) noexcept=0;

  private:
    std::weak_ptr<Observer<MSG, type_hint>> observer;
};

template <typename MSG, typename type_hint>
class UniqueObservable: public UniqueObservable_impl<MSG, type_hint>,
    public std::enable_shared_from_this<type_hint>
{
  public:
    UniqueObservable() noexcept=default;
    virtual ~UniqueObservable()=default;

  private:
    void notify_aux(
        const MSG& msg,
        std::weak_ptr<Observer<MSG, type_hint>> target
    ) noexcept override final{
        static_assert(std::derived_from<
            type_hint, UniqueObservable<MSG, type_hint>
        >);
        assert(!target.expired());
        target.lock()->onNotify(msg, this->shared_from_this());
    }
};

template <typename MSG>
class UniqueObservable<MSG, void>: public UniqueObservable_impl<MSG, void>{
  public:
    UniqueObservable() noexcept=default;
    virtual ~UniqueObservable()=default;

  private:
    void notify_aux(
        const MSG& msg,
        std::weak_ptr<Observer<MSG>> target
    ) noexcept override final{
        assert(!target.expired());
        target.lock()->onNotify(msg);
    }
};

class Sender{
  public:
    Sender() noexcept=default;
    virtual ~Sender()=default;

    void subscribe(std::weak_ptr<Receiver> r) noexcept{
        receivers.emplace_back(std::move(r));
    }
    void unsubscribe(std::weak_ptr<Receiver> r) noexcept{
        receivers.remove_if([&r](const auto& p){
            return not (p.owner_before(r) || r.owner_before(p));
        });
    }

    void notify() noexcept{
        remove_if_expired();

        for (auto &wo: receivers) {
            assert(!wo.expired());
            wo.lock()->handler();
        }
    }

  private:
    void remove_if_expired() noexcept{
        std::remove_if(
            receivers.begin(), receivers.end(),
            [](const auto &wp) {
                return wp.expired();
            }
        );
    }

  private:
    std::list<std::weak_ptr<Receiver>> receivers;
};

class UniqueSender{
  public:
    UniqueSender() noexcept=default;
    virtual ~UniqueSender()=default;

    void subscribe(std::weak_ptr<Receiver> r) noexcept{
        receiver=std::move(r);
    }
    void unsubscribe() noexcept{
        receiver.reset();
    }

    void notify() noexcept{
        if(receiver.expired()){
            receiver.reset();
            return;
        }

        receiver.lock()->handler();
    }

  private:
    std::weak_ptr<Receiver> receiver;
};
