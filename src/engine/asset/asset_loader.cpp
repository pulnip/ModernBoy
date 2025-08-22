#include <span>
#include "engine/asset/asset_loader.hpp"
#include "engine/asset/mesh_importer.hpp"
#include "engine/asset/material_importer.hpp"
#include "engine/engine.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Asset;

namespace{
    using namespace ModernBoy;
    using namespace ModernBoy::Asset;

    // "scheme:path" -> {scheme, path}
    std::pair<SchemeKind, std::string> splitSchemeAndPath(
        const std::string& id
    ){
        auto colon_pos = id.find(':');
        if(colon_pos == std::string::npos)
            return {SchemeKind::Unknown, ""};

        auto schemeStr = id.substr(0, colon_pos);
        auto pathStr = id.substr(colon_pos + 1);

        if(schemeStr == "file")
            return {SchemeKind::File, pathStr};
        else if(schemeStr == "embedded")
            return {SchemeKind::Embedded, pathStr};
        else
            return {SchemeKind::Unknown, pathStr};
    }

    // Get file extension (without dot).
    std::string getExtension(const std::string& path){
        auto ext_pos = path.find_last_of('.');
        if( ext_pos == std::string::npos ||
            ext_pos + 1 >= path.size() )
            return "";
        return path.substr(ext_pos + 1);
    }

    std::string usageToStr(TextureUsage tu){
        switch(tu){
            case BaseColor:
                return "BaseColor";
            case Normal:
                return "Normal";
            case MR:
                return "MR";
            case Emissive:
                return "Emissive";
            default:
                return "Unknown";
        }
    }
}

AssetLoader::AssetLoader(
    MeshTable& meshTable, MaterialSetTable& materialSetTable,
    SubmeshManager& submeshManager, MaterialManager& materialManager,
    ShaderManager& shaderManager, NativePtr renderContext)
:meshTable(meshTable), materialSetTable(materialSetTable),
submeshManager(submeshManager), materialManager(materialManager),
shaderManager(shaderManager), renderContext(renderContext){}

void AssetLoader::load(const SceneDescriptor& desc){
    load(desc.entities, desc.meshes);
}

void AssetLoader::load(const std::vector<Entity>& entities,
    const std::vector<MeshDescriptor>& meshes
){
    std::vector<MeshWork> meshWorks;
    std::vector<WorkItem> materialWorks, shaderWorks;

    for(const auto& entity: entities){
        if(entity.meshIndex != INVALID)
            collectWorkItems(entity.name, meshes[entity.meshIndex], meshWorks, materialWorks, shaderWorks);
    }

    // Execute meshWork
    for(const auto& work: meshWorks){
        switch(work.kind){
        case SchemeKind::File:
            processMeshFile(work);
            break;
        case SchemeKind::Embedded:
            processMeshEmbedded(work);
            break;
        default:
            table.try_emplace(work.id, issueID());
            AppWarn("id without scheme detected: {}", work.id);
            break;
        }
    }
    // Execute TextureWork
    for(const auto& work: materialWorks){
        switch(work.kind){
        case SchemeKind::File:
            processMaterial(work);
            break;
        case SchemeKind::Embedded:
            AppWarn("embedded texture not supported");
            break;
        default:
            AppWarn("id without scheme detected: {}", work.id);
            break;
        }
    }
    // Execute ShaderWork
    for(const auto& work: shaderWorks){
        switch(work.kind){
        case SchemeKind::File:
            processShader(work);
            break;
        case SchemeKind::Embedded:
            AppWarn("embedded shader not supported");
            break;
        default:
            AppWarn("id without scheme detected: {}", work.id);
            break;
        }
    }
}

void AssetLoader::collectWorkItems(
    const std::string& entityName, const MeshDescriptor& meshDesc,
    std::vector<MeshWork>& meshWorks, std::vector<WorkItem>& matWorks,
    std::vector<WorkItem>& shaderWorks
){
    {
        auto [kind, path] = splitSchemeAndPath(meshDesc.id);
        MeshWork meshWork{
            .entityName = entityName,
            .kind = kind,
            .id = meshDesc.id,
            .path = path,
            .material_override = meshDesc.material_override,
            .shader = meshDesc.shader
        };
        meshWorks.push_back(std::move(meshWork));

        bool hasMaterialOverride = !meshDesc.material_override.empty();
        if(kind==SchemeKind::Embedded && !hasMaterialOverride){
            AppError("embedded mesh {} requires at least one material",
                meshDesc.id);
            return;
        }
    }

    {
        for(const auto& matDesc: meshDesc.material_override){
            if(!matDesc.baseColor.empty()){
                auto [kind, path] = splitSchemeAndPath(matDesc.baseColor);
                if(kind != SchemeKind::Unknown){
                    WorkItem matWork{
                        .entityName = entityName,
                        .kind = kind,
                        .id = matDesc.baseColor,
                        .path = path,
                    };
                    matWorks.push_back(std::move(matWork));
                } else{
                    AppWarn("unknown seheme in baseColor: {}",
                        matDesc.baseColor);
                }
            }
        }
    }

    {
        const auto& sd = meshDesc.shader;
        auto [kind, path] = splitSchemeAndPath(sd.module_);
        if(kind != SchemeKind::Unknown){
            WorkItem shaderWork{
                .kind = kind,
                .id = sd.module_,
                .path = path,
            };
            shaderWorks.push_back(std::move(shaderWork));
        }
    }
}

// Process a "file" scheme on mesh id
void AssetLoader::processMeshFile(const MeshWork& item
){
    CookedMesh cooked = loadCookedOrImport(item.path);

    // submesh
    std::vector<SubmeshHandle> mesh(cooked.submeshInfoTable.size());
    for(Index i=0; i<cooked.submeshInfoTable.size(); ++i){
        auto submesh_it = table.find(std::format("{}:submesh{}", item.id, i));
        if(submesh_it != table.end()){
            mesh[i] = submeshManager.getHandle(submesh_it->second);
            continue;
        }

        const auto& submeshInfo = cooked.submeshInfoTable[i];

        std::span<Vertex> vertices(
            cooked.vertices.begin() + submeshInfo.verticesSectionIndex,
            submeshInfo.vertexCount
        );
        std::span<uint32_t> indices(
            cooked.indices.begin() + submeshInfo.indicesSectionIndex,
            submeshInfo.indexCount
        );
        
        auto submeshID = issueID();
        remember(std::format("{}:submesh{}", item.id, i), submeshID);

        auto submeshHandle = submeshManager.emplace(
            submeshID, renderContext, vertices, indices
        );
        mesh[i] = submeshHandle;
    }

    auto meshID = issueID();
    remember(std::format("{}:mesh", item.entityName),
        meshID);
    meshTable.try_emplace(meshID, mesh);

    // material
    std::unordered_map<std::string, uint32_t> materialSlotToIndex;
    for(Index i=0; i<cooked.materialInfoTable.size(); ++i)
        materialSlotToIndex.try_emplace(cooked.materialNameTable[i], i);
        // TODO. add material slot name table
        // materialSlotToIndex.try_emplace(cooked.materialSlotTable[i], i);

    std::vector<MaterialHandle> materialSet(cooked.materialInfoTable.size());

    for(const auto& matDesc: item.material_override){
        auto materialSetIndex = materialSlotToIndex.at(matDesc.targetSlot);

        auto mat_it = table.find(matDesc.baseColor);
        if(mat_it == table.end()){
            materialSet[materialSetIndex] = materialManager
                .getHandle(mat_it->second);

            continue;
        }

        auto matID = issueID();
        auto [matScheme, matPath] = splitSchemeAndPath(matDesc.baseColor);
        remember(matDesc.baseColor, matID);
        materialSet[materialSetIndex] = materialManager.emplace(
            matID, renderContext, matPath
        );

        // override, so doesn't need to iterate
        materialSlotToIndex.erase(matDesc.targetSlot);
    }

    for(const auto& [slotName, i]: materialSlotToIndex){
        const auto& materialInfo = cooked.materialInfoTable[i];
        const auto& materialName = cooked.materialNameTable[i];

        // check material already loaded
        auto mat_it = table.find(materialName);
        if(mat_it != table.end()){
            materialSet[i] = materialManager
                .getHandle(mat_it->second);

            continue;
        }

        // not loaded, so loading process start
        const auto& textureInfo = cooked.textureInfoTable[
            materialInfo.textureInfoTableIndex + 0];
        
        std::span<uint8_t> pixels(
            cooked.pixels.begin() + textureInfo.pixelSectionIndex,
            textureInfo.pixelCount
        );

        auto matID = issueID();
        remember(materialName, matID);
        auto handle = materialManager.emplace(
            matID, renderContext, pixels,
            textureInfo.width, textureInfo.height);
    }

    auto materialSetID = issueID();
    remember(std::format("{}:materialSet", item.entityName), materialSetID);
    materialSetTable.try_emplace(meshID, mesh);

    auto shader_it = table.find(item.shader.module_);
    if(shader_it != table.end())
        return;

    auto shaderID = issueID();
    remember(item.shader.module_, shaderID);
    auto handle = shaderManager.emplace(shaderID, renderContext,
        item.shader.vsFunc, item.shader.fsFunc, true);
    (void)handle;
}

CookedMesh AssetLoader::loadCookedOrImport(
    const std::string& path
){
    const auto ext = getExtension(path);
    if(ext == "mbmesh")
        return loadMeshFile(path);
    else
        return importMeshFile(path);
}

// Process an "embedded" scheme on mesh id
void AssetLoader::processMeshEmbedded(const MeshWork& item){
    std::vector<SubmeshHandle> mesh(1);

    for(Index i=0; i<1; ++i){
        auto submesh_it = table.find(item.id);
        if(submesh_it != table.end()){
            mesh[i] = submeshManager.getHandle(submesh_it->second);
            continue;
        }

        auto fittedMesh = loadEmbeddedMesh(item.path);

        std::span vertices = fittedMesh.vertices;
        std::span indices = fittedMesh.indices;

        auto meshID = issueID();
        remember(item.id, meshID);
        mesh[i] = submeshManager.emplace(
            meshID, renderContext, vertices, indices);
    }

    auto meshID = issueID();
    remember(std::format("{}:mesh", item.entityName), meshID);
    meshTable.try_emplace(meshID, mesh);

    std::vector<MaterialHandle> materialSet(1);
    std::unordered_map<std::string, uint32_t> materialSlotToIndex;
    materialSlotToIndex.try_emplace("*", 0);

    for(const auto& matDesc: item.material_override){
        auto materialSetIndex = materialSlotToIndex.at(matDesc.targetSlot);

        auto mat_it = table.find(matDesc.baseColor);
        if(mat_it != table.end()){
            materialSet[materialSetIndex] = materialManager
                .getHandle(mat_it->second);

            continue;
        }

        auto matID = issueID();
        auto [matScheme, matPath] = splitSchemeAndPath(matDesc.baseColor);
        remember(matDesc.baseColor, matID);
        materialSet[materialSetIndex] = materialManager.emplace(
            matID, renderContext, matPath);
    }

    auto materialSetID = issueID();
    remember(std::format("{}:materialSet", item.entityName), materialSetID);
    materialSetTable.try_emplace(materialSetID, materialSet);
}

void AssetLoader::processMaterial(const WorkItem& item){
    auto mat_it = table.find(item.id);
    if(mat_it == table.end()){
        auto matID = issueID();
        remember(item.id, matID);
        auto handle = materialManager.emplace(
            matID, renderContext, item.path);
        (void)handle;
    }

}

void AssetLoader::processShader(const WorkItem& item){
    auto shader_it = table.find(item.id);
    if(shader_it == table.end()){
        auto shaderID = issueID();
        remember(item.id, shaderID);
        auto handle = shaderManager.emplace(
            shaderID, renderContext);
        (void)handle;
    }
}


