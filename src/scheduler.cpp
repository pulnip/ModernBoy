#include <numeric>
#include <SDL3/SDL_timer.h>
#include "app_state.hpp"
#include "scheduler.hpp"

using namespace ModernBoy;

Scheduler::Scheduler(AppState& app):app(app){}

void Scheduler::prepareScheduling(){
    taskCounts.clear();
    taskGenerators.clear();
    generators.clear();
    schedule.clear();

    Uint64 now = SDL_GetTicks();
    if((now - lastTick) == 0){
        std::this_thread::sleep_for(
            std::chrono::milliseconds(1));
        now = SDL_GetTicks();
    }
    deltaTime = now - lastTick;
    lastTick = now;

    prepare(app.inputSystem);
    prepare(app.renderSystem);
    prepare(app.world);

    const size_t totalTasks = std::accumulate(
        taskCounts.cbegin(), taskCounts.cend(), 0);
    schedule.resize(totalTasks);
    std::vector<size_t> numScheduled(taskCounts.size(), 0);

    // Round-Robin with Proportional Fairness
    for(size_t step=0; step<totalTasks; ++step){
        double minRatio = double(numScheduled[0]) / taskCounts[0];
        size_t selectedIndex = 0;
        for(size_t i=1; i<taskCounts.size(); ++i){
            double ratio = double(numScheduled[i]) / taskCounts[i];
            if(ratio < minRatio){
                minRatio = ratio;
                selectedIndex = i;
            }
        }

        schedule.push_back(selectedIndex);
        ++numScheduled[selectedIndex];
    }
}

void Scheduler::prepareFrame(){
    for(auto idx: schedule){
        taskGenerators[idx].next();
    }

    bool all_done;
    do{
        all_done = true;
        for(auto& generator: taskGenerators){
            if(!generator.done()){
                generator.next();
                all_done = false;
            }
        }
    } while(!all_done);
}

void Scheduler::updateFrame(){
    for(auto idx: schedule){
        generators[idx].next();
    }

    bool all_done;
    do{
        all_done = true;
        for(auto& generator: generators){
            if(!generator.done()){
                generator.next();
                all_done = false;
            }
        }
    } while(!all_done);

    Uint64 now = SDL_GetTicks();

    constexpr auto TARGET_FPS = 60;
    Uint64 sleepDuration = 1000 / TARGET_FPS - 2;
    if(now - lastTick <= sleepDuration){
        std::this_thread::sleep_for(
            std::chrono::milliseconds(sleepDuration));
    }
}

uint64_t Scheduler::getDeltaTime() const{ return deltaTime; }