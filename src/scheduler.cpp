#include <numeric>
#include <thread>
#include <SDL3/SDL_timer.h>
#include "app_state.hpp"
#include "scheduler.hpp"

using namespace std::chrono;
using namespace std::chrono_literals;
using namespace ModernBoy;

Scheduler::Scheduler(AppState& app):app(app){}

void Scheduler::prepareScheduling(){

    taskCounts.clear();
    taskGenerators.clear();
    generators.clear();
    schedule.clear();

    auto now = steady_clock::now();

    if(now == lastTick){
        std::this_thread::sleep_for(
            milliseconds(1));
        now = steady_clock::now();
    }
    deltaTime = duration_cast<milliseconds>(
        now - lastTick);
    lastTick = now;

    app.prepare();

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

    auto now = steady_clock::now();

    constexpr auto TARGET_FPS = 60;
    DeltaTime sleepDuration = (1000ms/TARGET_FPS) - 2ms;
    if(now - lastTick <= sleepDuration){
        std::this_thread::sleep_for(sleepDuration);
    }
}

DeltaTime Scheduler::getDeltaTime() const{ return deltaTime; }