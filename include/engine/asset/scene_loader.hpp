#ifndef MODERNBOY_ASSET_SCENELOADER_HPP
#define MODERNBOY_ASSET_SCENELOADER_HPP

#include "engine/fwd.hpp"
#include "../../../src/game/game_fwd.hpp"

namespace ModernBoy::Asset
{
    class SceneLoader{
    public:
        SceneLoader(Game::EntityRegistry&);

        void loadScene(const SceneDescriptor&,
            const ResolveTable&);

    private:
        Game::EntityRegistry& registry;
    };
} // namespace ModernBoy::Asset

#endif // MODERNBOY_ASSET_SCENELOADER_HPP