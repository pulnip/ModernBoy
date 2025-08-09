#include "intent_service.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

void IntentService::write(const MoveIntent& intent){
    std::lock_guard lock(moveIntentMtx);
    moveIntents.push_back(intent);
}
void IntentService::write(const LookIntent& intent){
    std::lock_guard lock(lookIntentMtx);
    lookIntents.push_back(intent);
}

std::vector<MoveIntent> IntentService::drainMoveIntents(){
    std::vector<MoveIntent> out;
    {
        std::lock_guard lk(moveIntentMtx);
        out.swap(moveIntents);
    }
    return out;
}

std::vector<LookIntent> IntentService::drainLookIntents(){
    std::vector<LookIntent> out;
    {
        std::lock_guard lk(lookIntentMtx);
        out.swap(lookIntents);
    }
    return out;
}