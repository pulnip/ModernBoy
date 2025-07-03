#ifndef MODERNBOY_INPUT_SYSTEM_HPP
#define MODERNBOY_INPUT_SYSTEM_HPP

#include <chrono>
#include <map>
#include <unordered_set>
#include "fwd.hpp"
#include "common/alias.hpp"
#include "input/device.hpp"
#include "input/state.hpp"
#include "interface.hpp"
#include "util/dynamic_vector.hpp"

namespace ModernBoy::Input
{
    class System{
    public:
        System(AppState& app);
        ~System() = default;

        size_t yield_count() const noexcept;
        Generator<void> updateTask(DeltaTime dt);
        Generator<void> update(DeltaTime dt);

    private:
        AppState& app;

        State state;
        Device device;

        std::vector<InputTask> inputTasks;
    }; static_assert(Schedulable<System>);
} // namespace ModernBoy::Input

#endif // MODERNBOY_INPUT_SYSTEM_HPP