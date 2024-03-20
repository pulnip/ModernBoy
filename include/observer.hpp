#pragma once

#include <concepts>
#include <memory>

template <typename MSG, typename type_hint = void>
class Observable;
template <typename MSG, typename type_hint = void>
class UniqueObservable;

class Sender;
class UniqueSender;

template <typename MSG, typename Publisher=void>
class Observer{
    friend class Observable<MSG, Publisher>;
    friend class UniqueObservable<MSG, Publisher>;
  public:
    Observer() noexcept=default;
    virtual ~Observer()=default;

  private:
    virtual void onNotify(
        const MSG& msg,
        std::shared_ptr<Publisher> wo
    ) noexcept=0;
};

template <typename MSG>
class Observer<MSG, void>{
    friend class Observable<MSG>;
    friend class UniqueObservable<MSG>;

  public:
    Observer() noexcept=default;
    virtual ~Observer()=default;

  private:
    virtual void onNotify(const MSG& msg) noexcept=0;
};

class Receiver{
    friend class Sender;
    friend class UniqueSender;

  public:
    Receiver() noexcept=default;
    virtual ~Receiver()=default;

  private:
    virtual void handler() noexcept=0;
};
