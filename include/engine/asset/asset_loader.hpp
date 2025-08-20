#ifndef MODERNBOY_ASSET_ASSETLOADER_HPP
#define MODERNBOY_ASSET_ASSETLOADER_HPP

#include <map>
#include <vector>
#include "engine/fwd.hpp"
#include "engine/asset/asset_format.hpp"
#include "engine/asset/scene_parser.hpp"

namespace ModernBoy::Asset
{
    enum SchemeKind{
        File=0,
        Embedded=1,
        Unknown=2
    };

    struct MeshWork{
        SchemeKind kind=SchemeKind::Unknown;
        std::string id;
        std::string path;
        UUID uuid;
        std::vector<MaterialDescriptor> material_override;
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
        AssetLoader(SubmeshManager&, MeshManager&,
            TextureManager&, MaterialTable&, MaterialSetTable&,
            ShaderManager&, NativePtr renderContext);

        void load(const SceneDescriptor&);
        inline ResolveTable& get_table(){ return table; }
        inline const ResolveTable& get_table() const{ return table; }

    private:
        inline UUID issueID(){ return uuid++; }
        inline void remember(const std::string& id, UUID uuid){
            table.try_emplace(id, uuid);
        }

        void load(const std::vector<MeshDescriptor>&);

        // 1. load planning
        void collectWorkItems(const MeshDescriptor&,
            std::vector<MeshWork>& meshWorks,
            std::vector<WorkItem>& matWorks,
            std::vector<WorkItem>& shaderWorks
        );

        // 2. register to ResourceManager and load
        void processMeshFile(const MeshWork&);
        void processMeshEmbedded(const MeshWork&);
        void processMaterial(const WorkItem&);
        void processShader(const WorkItem&);

        auto createTexturesFromMesh(
            const CookedMesh&,
            const std::string& baseID
        ) -> std::vector<TextureHandle>;
        void createMaterialFromTextures(
            const std::vector<TextureHandle>& handles
        );

        auto loadCookedOrImport(
            const std::string& path
        ) -> CookedMesh;
        auto loadMaterial(
            const std::string& baseID
        ) -> std::vector<TextureHandle>;

        SubmeshManager& submeshManager;
        MeshManager& meshManager;
        TextureManager& textureManager;
        MaterialTable& materialTable;
        MaterialSetTable& materialSetTable;
        ShaderManager& shaderManager;
        NativePtr renderContext;

        ResolveTable table;
        // material 중복 제거
        std::map<std::vector<TextureHandle>, UUID,
            TextureVectorComparator> materialToUUID;
        UUID uuid = 0;
    };
} // namespace ModernBoy::Asset

#endif // MODERNBOY_ASSET_ASSETLOADER_HPP