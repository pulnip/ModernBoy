#pragma once

#include <memory>
#include "System/Input/Types.hpp"
#include "System/Input/EventReceiver.hpp"

class InputReceiver{
  protected:
    using Receiver=std::weak_ptr<EventReceiver>;

  public:
    virtual ~InputReceiver()=default;
    virtual void pollEvent()=0;
    virtual void subscribe(const Input::Key&, Receiver recv)=0;

  public:
    static std::shared_ptr<InputReceiver> null;
};

class NullInput: public InputReceiver{
    void pollEvent() override{}
    void subscribe(const Input::Key&, Receiver recv) override{}
};