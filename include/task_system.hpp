#ifndef MODERNBOY_TASK_SYSTEM_HPP
#define MODERNBOY_TASK_SYSTEM_HPP

#include <vector>
#include "fwd.hpp"
#include "component.hpp"

namespace ModernBoy
{
    class ComponentSystem{
    public:
        ComponentSystem(AppState& app):app(app){}

        template<typename Task>
        std::vector<Task> getTask();

    private:
        AppState& app;
    };
}

#endif // MODERNBOY_TASK_SYSTEM_HPP