#pragma once

template <typename MSG, typename T>
class Observable;

template <typename MSG, typename T = void>
class Observer {
    friend class Observable<MSG, T>;

  public:
    virtual ~Observer() = default;

  private:
    virtual void onNotify(MSG msg, T t) = 0;
};

template <typename MSG>
class Observer<MSG, void> {
    friend class Observable<MSG, void>;

  public:
    virtual ~Observer() = default;

  private:
    virtual void onNotify(MSG msg) = 0;
};
