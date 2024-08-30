#pragma once

#include <mutex>
#include <vector>
#include "InputState.hpp"
#include "Subsystem.hpp"

namespace ModernBoy{
    struct EventInput{
        ButtonState btn;
    };
    struct InputState{
        KeyboardState kbRef;
        ButtonState btn;
    };

    class InputSystem: public Subsystem<InputComponent, EventInput>{
      protected:
        std::vector<std::weak_ptr<InputComponent>> observers;

        mutable std::mutex mtx;

      public:
        virtual ~InputSystem()=default;

        virtual void prepare()=0;
        virtual void notifyReleased(const EventInput&)=0;

        void subscribe(std::weak_ptr<InputComponent> obs) override;
        void unsubscribe(std::weak_ptr<InputComponent> obs) override;
        void unsubscribeExpired() override;
    };
}
