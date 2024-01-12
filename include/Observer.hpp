#pragma once

#include <concepts>

template <typename MSG, typename type_hint = void>
class Observable;

template <typename MSG, typename Observable_t = void>
class Observer {
    friend class Observable<MSG, Observable_t>;

  protected:
    using MSG_t = MSG;
    using spObservable = std::shared_ptr<Observable_t>;

  public:
    virtual ~Observer() = default;

  private:
    virtual void onNotify(MSG_t msg, spObservable wo) = 0;
};

template <typename MSG>
class Observer<MSG, void> {
    friend class Observable<MSG, void>;

  protected:
    using MSG_t = MSG;

  public:
    virtual ~Observer() = default;

  private:
    virtual void onNotify(MSG_t msg) = 0;
};
