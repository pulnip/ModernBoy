#ifndef MODERNBOY_GAME_INTENT_SERVICE_HPP
#define MODERNBOY_GAME_INTENT_SERVICE_HPP

#include <mutex>
#include <span>
#include <vector>
#include "engine/fwd.hpp"
#include "game_fwd.hpp"

namespace ModernBoy::Game
{
    struct MoveIntent{
        Vec3 move;
    };
    struct LookIntent{
        float yaw;
        float pitch;
    };

    class IntentService{
    private:
        std::vector<MoveIntent> moveIntents;
        std::vector<LookIntent> lookIntents;

        std::mutex moveIntentMtx, lookIntentMtx;

    public:
        IntentService() = default;
        ~IntentService() = default;

        void write(const MoveIntent& intent);
        void write(const LookIntent& intent);
    
        std::vector<MoveIntent> drainMoveIntents();
        std::vector<LookIntent> drainLookIntents();
    };
}

#endif // MODERNBOY_GAME_INTENT_SERVICE_HPP