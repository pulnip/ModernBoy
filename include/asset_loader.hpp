#ifndef MODERNBOY_ASSET_LOADER_HPP
#define MODERNBOY_ASSET_LOADER_HPP

#include <string>
#include "fwd.hpp"
#include "game/game_fwd.hpp"
#include "game/component.hpp"

namespace ModernBoy
{
    struct ModelDescriptor{
        std::string mesh;
        std::string texture;
        std::string shader;
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
        AssetLoader(AppState& app);

        void loadAsset(const std::string& fileName);
        void loadAction(const std::string& fileName);

    private:
        Game::Model load(const ModelDescriptor&);
        Game::ScriptObject load(const ScriptDescriptor&);

        void load(const ModuleDescriptor&);

        AppState& app;
#if defined(USE_DIRECTX)
#elif defined(USE_METAL)
        NativePtr metalView;
#elif defined(USE_OPENGL)
#endif
    };
} // namespace ModernBoy

#endif // MODERNBOY_ASSET_LOADER_HPP