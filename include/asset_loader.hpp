#ifndef MODERNBOY_ASSET_LOADER_HPP
#define MODERNBOY_ASSET_LOADER_HPP

#include <string>
#include "fwd.hpp"

namespace ModernBoy
{
    class AssetLoader{
    public:
        AssetLoader(AppState& app);

        void loadAsset(const std::string& fileName);
        void loadAction(const std::string& fileName);

    private:
        AppState& app;
#if defined(USE_DIRECTX)
#elif defined(USE_METAL)
        NativePtr metalView;
#elif defined(USE_OPENGL)
#endif
    };
} // namespace ModernBoy

#endif // MODERNBOY_ASSET_LOADER_HPP