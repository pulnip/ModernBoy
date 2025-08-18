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
        AssetLoader(MeshManager&,
            NativePtr renderContext);

        void load(const SceneDescriptor&);
        inline ResolveTable& get_table(){ return table; }
        inline const ResolveTable& get_table() const{ return table; }

    private:
        inline UUID issueID(){ return uuid++; }

        void load(const std::vector<MeshDescriptor>&);

        MeshManager& meshManager;
        NativePtr renderContext;
        UUID uuid = 0;
        ResolveTable table;
    };
} // namespace ModernBoy::Asset

#endif // MODERNBOY_ASSET_ASSETLOADER_HPP