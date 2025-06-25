#ifndef MODERNBOY_INPUT_SYSTEM_HPP
#define MODERNBOY_INPUT_SYSTEM_HPP

#include <chrono>
#include <map>
#include <unordered_set>
#include "fwd.hpp"
#include "common/alias.hpp"

namespace ModernBoy::Input
{
    class System{
    public:
        System(AppState& app);
        ~System() = default;

        void update(DeltaTime dt);

    private:
        AppState& app;
    };
} // namespace ModernBoy::Input

#endif // MODERNBOY_INPUT_SYSTEM_HPP