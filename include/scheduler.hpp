#ifndef MODERNBOY_SCHEDULER_HPP
#define MODERNBOY_SCHEDULER_HPP

#include <array>
#include <variant>
#include <vector>
#include "fwd.hpp"
#include "util/generator.hpp"

namespace ModernBoy
{
    struct InputTaskCommand{};
    struct WorldUpdateCommand{};
    struct PhysicsTaskCommand{};
    struct RenderTaskCommand{};

    using TaskCommand = std::variant<
        InputTaskCommand,
        WorldUpdateCommand,
        PhysicsTaskCommand,
        RenderTaskCommand
    >;

    class Scheduler{
    public:
        Scheduler(AppState& app);

        void prepareScheduling();
        // task update phase
        void prepareFrame();
        // entity update phase
        void updateFrame();

        uint64_t getDeltaTime() const;

    private:
        AppState& app;

        std::vector<Generator<void>> taskGenerators;
        std::vector<Generator<void>> generators;
        std::vector<size_t> schedule;

        uint64_t lastTick;
        uint64_t deltaTime;
    };
} // namespace ModernBoy

#endif // MODERNBOY_SCHEDULER_HPP