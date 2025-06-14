#ifndef MODERNBOY_TASK_MANAGER_HPP
#define MODERNBOY_TASK_MANAGER_HPP

#include <algorithm>
#include <functional>
#include <vector>

namespace ModernBoy
{
    template<typename Task>
    class TaskManager{
    private:
        std::vector<Task> components;

    public:
        template<typename... Args>
        void create(Args... args){
            components.emplace_back(Task(args...));
        }

        auto getAll() const{
            std::vector<std::reference_wrapper<const Task>> wrapped;

            std::for_each(components.cbegin(), components.cend(),
                [&wrapped](const auto& comp){
                    wrapped.emplace_back(std::cref(comp));
                }
            );

            return wrapped;
        }
    };
} // namespace ModernBoy

#endif // MODERNBOY_TASK_MANAGER_HPP