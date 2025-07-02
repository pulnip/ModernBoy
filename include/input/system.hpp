#ifndef MODERNBOY_INPUT_SYSTEM_HPP
#define MODERNBOY_INPUT_SYSTEM_HPP

#include <chrono>
#include <map>
#include <unordered_set>
#include "fwd.hpp"
#include "common/alias.hpp"
#include "input/device.hpp"
#include "input/state.hpp"
#include "util/generator.hpp"
#include "util/dynamic_vector.hpp"

namespace ModernBoy::Input
{
    class System{
    public:
        System(AppState& app);
        ~System() = default;

        void update(DeltaTime dt);

        size_t yield_count() const noexcept;
        Generator<void> updateV2(DeltaTime dt);


    private:
        AppState& app;

        State state;
        Device device;
    };
} // namespace ModernBoy::Input

#endif // MODERNBOY_INPUT_SYSTEM_HPP