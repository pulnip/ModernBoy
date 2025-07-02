#include <numeric>
#include "app_state.hpp"
#include "scheduler.hpp"

using namespace ModernBoy;

Scheduler::Scheduler(AppState& app):app(app){}

void Scheduler::prepareFrame(){
    generators.clear();
    schedule.clear();

    // update timer
    std::vector<size_t> taskCounts;

    generators.push_back(app.inputSystem.updateV2(Uint64(0)));
    taskCounts.push_back(app.inputSystem.yield_count());


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
}