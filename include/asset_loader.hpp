#ifndef MODERNBOY_ASSET_LOADER_HPP
#define MODERNBOY_ASSET_LOADER_HPP

#include <expected>
#include <string>
#include "fwd.hpp"
#include "game/game_fwd.hpp"

namespace ModernBoy
{
    enum class parse_error{
        invalid_table,
        omitted_column
    };
    struct ScriptSection{
        std::string moduleName;
    };

    class AssetLoader{
    public:
        AssetLoader(AppState& app);

        void loadAsset(const std::string& fileName);
        void loadAction(const std::string& fileName);

    private:
        std::expected<Game::ActionComponent, parse_error>
            makeActionComponent(const ScriptSection& section);

        AppState& app;
#if defined(USE_DIRECTX)
#elif defined(USE_METAL)
        NativePtr metalView;
#elif defined(USE_OPENGL)
#endif
    };
} // namespace ModernBoy

#endif // MODERNBOY_ASSET_LOADER_HPP