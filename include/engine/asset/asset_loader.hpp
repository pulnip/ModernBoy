#ifndef MODERNBOY_ASSET_ASSETLOADER_HPP
#define MODERNBOY_ASSET_ASSETLOADER_HPP

#include <unordered_map>
#include "engine/fwd.hpp"
#include "engine/asset/scene_parser.hpp"
#include "engine/asset/mesh_importer.hpp"

namespace ModernBoy::Asset
{
    using ResolveTable = std::unordered_map<std::string, UUID>;

    class AssetLoader{
    public:
        AssetLoader(Engine&);

        void load(const SceneDescriptor&);

    private:
        inline UUID issueID(){ return uuid++; }
        inline ResolveTable& get(){ return table; }

        void load(const std::vector<MeshDescriptor>&);

        Engine& engine;
        UUID uuid = 0;
        ResolveTable table;
    };
} // namespace ModernBoy::Asset

#endif // MODERNBOY_ASSET_ASSETLOADER_HPP