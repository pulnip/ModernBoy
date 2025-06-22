#ifndef MODERNBOY_TASK_SYSTEM_HPP
#define MODERNBOY_TASK_SYSTEM_HPP

#include <algorithm>
#include <functional>
#include <vector>

namespace ModernBoy
{
    template<typename Component, typename Task>
    class TaskSystem{
    private:
        std::vector<Component> components;

    public:
        template<typename... Args>
        void create(Args... args){
            components.emplace_back(args...);
        }

        auto getAll() const{
            std::vector<Task> tasks;

            std::for_each(components.cbegin(), components.cend(),
                [&tasks](const auto& component){
                    auto newTasks = component.getTasks();
                    tasks.append_range(newTasks);
                }
            );

            return tasks;
        }
    };
} // namespace ModernBoy

#endif // MODERNBOY_TASK_SYSTEM_HPP