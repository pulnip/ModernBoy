#pragma once

#include <memory>
#include "fwd.hpp"

namespace ModernBoy{
    template<typename Observer, typename MSG>
    class Subsystem{
      public:
        Subsystem()=default;
        virtual ~Subsystem()=default;

        virtual void notify(const MSG& msg)=0;

        virtual void subscribe(std::weak_ptr<Observer> obs)=0;
        virtual void unsubscribe(std::weak_ptr<Observer> obs)=0;
        virtual void unsubscribeExpired()=0;
    };
}
