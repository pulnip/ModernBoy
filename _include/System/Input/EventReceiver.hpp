#pragma once

#include "observer.hpp"
#include "System/Input/Types.hpp"

class EventReceiver: public Observer<Input::Event>{
  public:
    virtual void onNotify(const Input::Event& e) noexcept override=0;
};