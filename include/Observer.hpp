#pragma once

#include <concepts>
#include <memory>

template <typename MSG, typename type_hint = void>
class Observable;
template <typename MSG, typename type_hint = void>
class UniqueObservable;

template <typename MSG, typename Publisher=void>
class Observer{
    friend class Observable<MSG, Publisher>;
    friend class UniqueObservable<MSG, Publisher>;
  public:
    Observer() noexcept=default;
    virtual ~Observer()=default;

  private:
    virtual void onNotify(
        MSG msg,
        std::shared_ptr<Publisher> wo
    )=0;
};

template <typename MSG>
class Observer<MSG, void>{
    friend class Observable<MSG, void>;
    friend class UniqueObservable<MSG, void>;

  public:
    Observer() noexcept=default;
    virtual ~Observer()=default;

  private:
    virtual void onNotify(MSG msg)=0;
};
