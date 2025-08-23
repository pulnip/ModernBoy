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
        std::string entityName;
        SchemeKind kind=SchemeKind::Unknown;
        std::string id;
        std::string path;
        std::vector<MaterialDescriptor> material_override;
        ShaderDescriptor shader;
    };

    struct WorkItem{
        std::string entityName;
        SchemeKind kind=SchemeKind::Unknown;
        std::string id;
        std::string path;
    };

    using MeshWorks = std::vector<MeshWork>;
    using MaterialWork = WorkItem;
    using MaterialWorks = std::vector<MaterialWork>;
    using ShaderWork = WorkItem;
    using ShaderWorks = std::vector<ShaderWork>;

    class AssetLoader{
    public:
        AssetLoader(
            MeshTable&, MaterialSetTable&,
            SubmeshManager&, MaterialManager&,
            ShaderManager&, NativePtr renderContext);

        void load(const SceneDescriptor&);
        inline ResolveTable& get_table(){ return table; }
        inline const ResolveTable& get_table() const{ return table; }

    private:
        inline UUID issueID(){ return uuid++; }
        inline void remember(const std::string& id, UUID uuid){
            table.try_emplace(id, uuid);
        }

        void load(const EntityDescriptors&,
            const MeshDescriptors&);

        // 1. load planning
        auto collectMeshWork(
            const std::string& entityName,
            const MeshDescriptor&
        ) -> MeshWork;
        auto collectMaterialWorks(
            const std::string& entityName,
            const MeshDescriptor&
        ) -> MaterialWorks;
        auto collectShaderWork(
            const std::string& entityName,
            const MeshDescriptor&
        ) -> ShaderWork;

        // 2. register to ResourceManager and load
        bool bindMeshToEntity(const std::string& entityName, UUID);
        bool bindMaterialSetToEntity(const std::string& entityName, UUID);
        bool bindShaderToEntity(const std::string& entityName, UUID);

        CookedMesh loadCookedMeshFor(const MeshWork&);
        auto loadSubmeshes(
            const CookedMesh&
        )->UUID;
        auto loadMaterialSet(
            const std::string& meshFileName,
            const CookedMesh&,
            const MaterialDescriptors&
        )->UUID;
        auto loadShader(
            const ShaderDescriptor&
        )->UUID;

        void executeMeshFileWork(const MeshWork&);

        void processMeshEmbedded(const MeshWork&);
        void processMaterial(const WorkItem&);
        void processShader(const WorkItem&);

        auto loadCookedOrImport(
            const std::string& path
        ) -> CookedMesh;

        MeshTable& meshTable;
        MaterialSetTable& materialSetTable;
        SubmeshManager& submeshManager;
        MaterialManager& materialManager;
        ShaderManager& shaderManager;
        NativePtr renderContext;

        ResolveTable table;
        UUID uuid = 0;
    };
} // namespace ModernBoy::Asset

#endif // MODERNBOY_ASSET_ASSETLOADER_HPP