#ifndef MODERNBOY_GAME_SCHEDULER_HPP
#define MODERNBOY_GAME_SCHEDULER_HPP

#include <vector>
#include "thread/generator.hpp"
#include "interface.hpp"
#include "fwd.hpp"
#include "game_fwd.hpp"

namespace ModernBoy::Game
{
    class Scheduler{
    public:
        Scheduler(Context& world);

        void prepareScheduling(DeltaTime);
        void startUpdate();

        void prepare(Schedulable auto& system){
            numYield.push_back(system.yield_count());
            generators.push_back(system.update(deltaTime));
        }

    private:
        Context& world;

        std::vector<size_t> numYield;
        std::vector<Generator<void>> generators;

        DeltaTime deltaTime;
    };
}

#endif // MODERNBOY_GAME_SCHEDULER_HPP