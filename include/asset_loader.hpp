#ifndef MODERNBOY_ASSET_LOADER_HPP
#define MODERNBOY_ASSET_LOADER_HPP

#include <string>
#include "fwd.hpp"

namespace ModernBoy
{
    class AssetLoader{
        AppState& app;

    public:
        AssetLoader(AppState& app);

        void loadActors(const std::string& fileName);
        void loadCamera(const std::string& fileName);
    };
} // namespace ModernBoy

#endif // MODERNBOY_ASSET_LOADER_HPP