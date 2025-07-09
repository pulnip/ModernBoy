#include <limits>
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
    auto now = std::chrono::time_point_cast<
        std::chrono::microseconds>(steady_clock::now());

    if(now == lastTick){
        std::this_thread::sleep_for(
            milliseconds(1));
        now = std::chrono::time_point_cast<
            std::chrono::microseconds>(steady_clock::now());
    }
    deltaTime = duration_cast<milliseconds>(
        now - lastTick);
    lastTick = now;

    taskSchedule.clear();
    updateSchedule.clear();
    taskGenerators.clear();
    updateGenerators.clear();
    taskCounts.clear();
    updateCounts.clear();

    const size_t totalTasks = std::accumulate(
        taskCounts.cbegin(), taskCounts.cend(), 0);
    taskSchedule.resize(totalTasks);
    std::vector<size_t> numScheduled(taskCounts.size(), 0);

    // Round-Robin with Proportional Fairness
    for(size_t step=0; step<totalTasks; ++step){
        double minRatio = 1;
        size_t selectedIndex = std::numeric_limits<size_t>::max();
        for(size_t i=0; i<taskCounts.size(); ++i){
            double ratio = double(numScheduled[i]) / taskCounts[i];
            if(ratio < minRatio){
                minRatio = ratio;
                selectedIndex = i;
            }
        }
        if(selectedIndex == std::numeric_limits<size_t>::max())
            continue;

        taskSchedule.push_back(selectedIndex);
        ++numScheduled[selectedIndex];
    }

    AppTrace("Scheduling   T.U.P., Num: {}, total   Tasks: {}",
        updateGenerators.size(), numScheduled.size(), totalTasks);

    const size_t totalUpdates = std::accumulate(
        updateCounts.cbegin(), updateCounts.cend(), 0);

    numScheduled.clear();
    numScheduled.reserve(updateCounts.size());
    std::fill(numScheduled.begin(), numScheduled.end(), 0);

    // Round-Robin with Proportional Fairness
    for(size_t step=0; step<totalUpdates; ++step){
        double minRatio = 1;
        size_t selectedIndex = std::numeric_limits<size_t>::max();
        for(size_t i=0; i<updateCounts.size(); ++i){
            double ratio = double(numScheduled[i]) / updateCounts[i];
            if(ratio < minRatio){
                minRatio = ratio;
                selectedIndex = i;
            }
        }
        if(selectedIndex == std::numeric_limits<size_t>::max())
            continue;

        updateSchedule.push_back(selectedIndex);
        ++numScheduled[selectedIndex];
    }
    AppTrace("Scheduling   E.U.P., Num: {}, total Updates: {}",
        updateGenerators.size(), numScheduled.size(), totalUpdates);
}

void Scheduler::prepareFrame(){
    AppTrace("T.U.P., Num Generators: {}, Num Scheduled: {}",
        taskGenerators.size(), taskSchedule.size());
    app.on<Event::OnFrameStart>();

    for(auto idx: taskSchedule){
        if(taskGenerators[idx].done())
            continue;
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

    app.on<Event::OnFrameEnd>();
    AppTrace("T.U.P., Finished");
}

void Scheduler::updateFrame(){
    AppTrace("E.U.P., Num Generators: {}, Num Scheduled: {}",
        updateGenerators.size(), updateSchedule.size());

    for(auto idx: updateSchedule){
        if(updateGenerators[idx].done())
            continue;
        updateGenerators[idx].next();
    }

    bool all_done;
    do{
        all_done = true;
        for(auto& generator: updateGenerators){
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
    AppTrace("E.U.P., Finished");
}

DeltaTime Scheduler::getDeltaTime() const{ return deltaTime; }