#ifndef MODERNBOY_SCHEDULER_HPP
#define MODERNBOY_SCHEDULER_HPP

#include <array>
#include <variant>
#include <vector>
#include "fwd.hpp"
#include "interface.hpp"

namespace ModernBoy
{
    class Scheduler{
    public:
        Scheduler(AppState& app);

        void prepareScheduling();
        // task update phase
        void prepareFrame();
        // entity update phase
        void updateFrame();

        DeltaTime getDeltaTime() const;

        void prepareAllPhase(Schedulable auto& schedulable){
            auto count = schedulable.yield_count();

            taskCounts.push_back(count);
            updateCounts.push_back(count);

            taskGenerators.push_back(schedulable.updateTask(deltaTime));
            generators.push_back(schedulable.update(deltaTime));
        }
        void prepareTaskPhase(Schedulable auto& schedulable){
            taskCounts.push_back(schedulable.yield_count());
            taskGenerators.push_back(schedulable.updateTask(deltaTime));

        }
        void prepareUpdatePhase(Schedulable auto& schedulable){
            updateCounts.push_back(schedulable.yield_count());
            generators.push_back(schedulable.update(deltaTime));
        }

    private:
        AppState& app;

        std::vector<size_t> taskCounts;
        std::vector<size_t> updateCounts;
        std::vector<Generator<void>> taskGenerators;
        std::vector<Generator<void>> generators;
        std::vector<size_t> schedule;

        Timepoint lastTick;
        DeltaTime deltaTime;
    };
} // namespace ModernBoy

#endif // MODERNBOY_SCHEDULER_HPP