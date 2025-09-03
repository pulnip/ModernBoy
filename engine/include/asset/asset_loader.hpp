#ifndef MODERNBOY_ASSET_ASSETLOADER_HPP
#define MODERNBOY_ASSET_ASSETLOADER_HPP

#include <map>
#include <vector>
#include "fwd.hpp"
#include "asset/asset_format.hpp"
#include "asset/mesh_importer.hpp"
#include "asset/scene_parser.hpp"

namespace ModernBoy::Asset
{
    enum SchemeKind{
        File=0,
        Embedded=1,
        Unknown=2
    };

    struct MeshMaterialWork{
        std::string entityName;
        std::string meshID;
        std::vector<MaterialDescriptor> material_override;
    };

    struct ShaderWork{
        std::string entityName;
        ShaderDescriptor shader;
    };

    using MeshMaterialWorks = std::vector<MeshMaterialWork>;
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

        // 2. register to ResourceManager and load
        void executeMeshMaterialWork(
            const MeshMaterialWork&);
        void executeMaterialSetWork(
            const std::string& entityName,
            const std::string& meshName,
            const CookedMesh&,
            const MaterialDescriptors& material_override);
        void executeMaterialSetWork(
            const std::string& entityName,
            const std::string& meshName,
            const MaterialDescriptors& material_override);
        void executeShaderWork(const ShaderWork&);

        bool bindMeshToEntity(
            const std::string& entityName,
            UUID);
        bool bindMaterialSetToEntity(
            const std::string& entityName,
            UUID);
        bool bindShaderToEntity(
            const std::string& entityName,
            UUID);

        auto loadSubmeshes(
            const CookedMesh&
        )->UUID;
        auto loadSubmeshes(
            const EmbeddedMesh&
        )->UUID;
        auto loadMaterialSet(
            const std::string& meshFileName,
            const CookedMesh&,
            const MaterialDescriptors&
        )->UUID;
        auto loadMaterialSet(
            const std::string& meshFileName,
            const MaterialDescriptors&
        )->UUID;
        auto loadShader(
            const ShaderDescriptor&
        )->UUID;

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

    // Test helpers
    auto countLoadedResources(const ResolveTable&)->size_t;
} // namespace ModernBoy::Asset

#endif // MODERNBOY_ASSET_ASSETLOADER_HPP