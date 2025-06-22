#ifndef MODERNBOY_ASSET_LOADER_HPP
#define MODERNBOY_ASSET_LOADER_HPP

#include <string>
#include "fwd.hpp"

namespace ModernBoy
{
    class AssetLoader{
        AppState& app;

        uint32_t id_seed = 0;
        uint32_t issueID(){ return id_seed++; }

    public:
        AssetLoader(AppState& app);

        void loadActors(const std::string& fileName);
        void loadCamera(const std::string& fileName);
    };
} // namespace ModernBoy

#endif // MODERNBOY_ASSET_LOADER_HPP