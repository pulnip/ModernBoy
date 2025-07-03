#ifndef MODERNBOY_SCHEDULER_HPP
#define MODERNBOY_SCHEDULER_HPP

#include <array>
#include <variant>
#include <vector>
#include "fwd.hpp"
#include "interface.hpp"

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
        void prepare(Schedulable auto& schedulable){
            taskCounts.push_back(schedulable.yield_count());
            taskGenerators.push_back(schedulable.updateTask(deltaTime));
            generators.push_back(schedulable.update(deltaTime));
        }

        AppState& app;

        std::vector<size_t> taskCounts;
        std::vector<Generator<void>> taskGenerators;
        std::vector<Generator<void>> generators;
        std::vector<size_t> schedule;

        uint64_t lastTick;
        uint64_t deltaTime;
    };
} // namespace ModernBoy

#endif // MODERNBOY_SCHEDULER_HPP