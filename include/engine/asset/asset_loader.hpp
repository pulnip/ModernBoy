#ifndef MODERNBOY_ASSET_ASSETLOADER_HPP
#define MODERNBOY_ASSET_ASSETLOADER_HPP

#include <set>
#include <vector>
#include "engine/fwd.hpp"
#include "engine/asset/scene_parser.hpp"
#include "engine/asset/mesh_importer.hpp"

namespace ModernBoy::Asset
{
    enum SchemeKind{
        File=0,
        Embedded=1,
        Unknown=2
    };

    struct WorkItem{
        SchemeKind kind=SchemeKind::Unknown;
        std::string id;
        std::string path;
        UUID uuid;
    };

    struct TextureVectorComparator{
        inline bool operator()(
            const std::vector<TextureHandle>& lhs,
            const std::vector<TextureHandle>& rhs
        ) const{
            if(lhs.size() != rhs.size())
                return lhs.size() < rhs.size();
            for(Index i=0; i<lhs.size(); ++i){
                if(lhs[i] != rhs[i])
                    return lhs[i] < rhs[i];
            }
            return false;
        }
    };

    class AssetLoader{
    public:
        AssetLoader(MeshManager&, TextureManager&,
            ShaderManager&, NativePtr renderContext);

        void load(const SceneDescriptor&);
        inline ResolveTable& get_table(){ return table; }
        inline const ResolveTable& get_table() const{ return table; }

    private:
        inline UUID issueID(){ return uuid++; }
        UUID issueUUID();

        void load(const std::vector<MeshDescriptor>&);
        void collectWorkItemFromDescriptor(
            const std::string& id,
            std::vector<WorkItem>&);
        CookedMesh loadCookedOrImport(const std::string& path);
        std::vector<UUID> createTexturesFromMesh(
            const CookedMesh&, const std::string& baseID);
        void createMaterialFromMesh(
            const CookedMesh&, const std::vector<UUID>& texIDs);
        void processMeshFile(const WorkItem&);
        void processMeshEmbedded(const WorkItem&);
        void processTexture(const WorkItem&);
        void processShader(const WorkItem&);

        MeshManager& meshManager;
        TextureManager& textureManager;
        ShaderManager& shaderManager;
        std::set<std::vector<TextureHandle>, TextureVectorComparator> materialSets;
        NativePtr renderContext;
        UUID uuid = 0;
        ResolveTable table;
    };
} // namespace ModernBoy::Asset

#endif // MODERNBOY_ASSET_ASSETLOADER_HPP