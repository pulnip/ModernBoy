#ifndef MODERNBOY_ASSET_LOADER_HPP
#define MODERNBOY_ASSET_LOADER_HPP

#include <string>
#include "fwd.hpp"

namespace ModernBoy
{
    class AssetLoader{
    public:
        AssetLoader(AppState& app);

        void loadActors(const std::string& fileName);
        void loadScripts(const std::string& fileName);

    private:
        AppState& app;
    };
} // namespace ModernBoy

#endif // MODERNBOY_ASSET_LOADER_HPP