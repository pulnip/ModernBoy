#ifndef MODERNBOY_INPUT_SYSTEM_HPP
#define MODERNBOY_INPUT_SYSTEM_HPP

#include <chrono>
#include <map>
#include <unordered_set>
#include "fwd.hpp"
#include "common/alias.hpp"

namespace ModernBoy::Input
{
    class System{
    public:
        System(AppState& app);
        ~System() = default;

        void update(DeltaTime dt);
        size_t numInputTask();

        void ignoreActor(EntityID actor);
        void unignoreActor(EntityID actor);

    private:
        AppState& app;

        using IgnoredActors = std::unordered_set<EntityID>;
        using IgnoredIndex = std::unordered_set<Index>;
        using IgnoranceInfo = std::map<ArchetypeBit, IgnoredIndex>;

        void removeDangledActor();

        IgnoranceInfo ignoranceInfo;
    };
} // namespace ModernBoy::Input

#endif // MODERNBOY_INPUT_SYSTEM_HPP