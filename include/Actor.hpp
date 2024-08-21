#pragma once

#include <vector>
#include <memory>
#include "Attributes.hpp"
#include "fwd.hpp"

namespace ModernBoy{
    class Actor{
        std::vector<std::shared_ptr<Component>> components;

      public:
        Transform transform{};

      public:
        Actor(const Core& core);
        virtual ~Actor();
        virtual void update(float dt);

        void addComponent(std::shared_ptr<Component> comp);
    };
}
