#pragma once

#include <memory>

template <typename T>
class Observable;

enum class Observable_msg;

template <typename T>
class Observer {
    friend class Observable<T>;

  public:
    virtual ~Observer() = default;

  private:
    virtual void onNotify(std::shared_ptr<T> observable, Observable_msg msg) = 0;
};
