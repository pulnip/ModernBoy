#ifndef MODERNBOY_ASSET_LOADER_HPP
#define MODERNBOY_ASSET_LOADER_HPP

#include <string>
#include "engine/fwd.hpp"
#include "game/game_fwd.hpp"
#include "game/component.hpp"

namespace ModernBoy
{
    struct ModelDescriptor{
        std::string mesh;
        std::string texture;
        std::string vsFunc;
        std::string fsFunc;
    };
    struct ScriptDescriptor{
        std::string module_;
        std::string type;
    };

    struct ModuleDescriptor{
        std::string module_;
        std::vector<std::string> files;
        // std::vector<std::string> funcs;
    };

    class AssetLoader{
    public:
        AssetLoader(Engine& engine);

        void loadAsset(const std::string& fileName);
        void loadAction(const std::string& fileName);

    private:
        Game::Model load(const ModelDescriptor&);
        Game::ScriptObject load(const ScriptDescriptor&);

        void load(const ModuleDescriptor&);

        Engine& engine;
    };
} // namespace ModernBoy

#endif // MODERNBOY_ASSET_LOADER_HPP