#include <span>
#include "engine/asset/asset_loader.hpp"
#include "engine/asset/material_importer.hpp"
#include "engine/engine.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Asset;

namespace{
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

    auto makeMaterialSetID(const std::string& baseID,
        const MaterialDescriptors& descs
    ){
        auto materialSetID = baseID;
        for(const auto& desc: descs){
            materialSetID = std::format("{}:{}",
                materialSetID, desc.baseColor);
        }

        return materialSetID;
    }

    auto makeShaderID(const ShaderDescriptor& desc){
        return std::format("{}:{},{}", desc.module_,
            desc.vsFunc, desc.fsFunc);
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

void AssetLoader::load(const EntityDescriptors& entities,
    const MeshDescriptors& meshes
){
    MeshWorks meshWorks;
    MaterialWorks materialWorks;
    ShaderWorks shaderWorks;

    meshWorks.reserve(entities.size());
    materialWorks.reserve(entities.size());
    shaderWorks.reserve(entities.size());

    for(const auto& entity: entities){
        if(entity.meshIndex != INVALID){
            meshWorks.push_back(collectMeshWork(
                entity.name, meshes[entity.meshIndex]));
            materialWorks.append_range(collectMaterialWorks(
                entity.name, meshes[entity.meshIndex]));
            shaderWorks.push_back(collectShaderWork(
                entity.name, meshes[entity.meshIndex]));
        }
    }

    // Execute meshWork
    for(const auto& work: meshWorks){
        switch(work.kind){
        case SchemeKind::File:
            executeMeshFileWork(work);
            break;
        case SchemeKind::Embedded:
            executeMeshEmbeddedWork(work);
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

auto AssetLoader::collectMeshWork(
    const std::string& entityName,
    const MeshDescriptor& desc
)->MeshWork{
    auto [kind, path] = splitSchemeAndPath(desc.id);
    MeshWork work{
        .entityName = entityName,
        .kind = kind,
        .id = desc.id,
        .path = path,
        .material_override = desc.material_override,
        .shader = desc.shader
    };

    bool hasMaterialOverride = !desc.material_override.empty();
    if(kind==SchemeKind::Embedded && !hasMaterialOverride){
        AppError("embedded mesh {} requires at least one material",
            desc.id);
    }
    return work;
}

auto AssetLoader::collectMaterialWorks(
    const std::string& entityName, const MeshDescriptor& desc
)-> MaterialWorks{
    MaterialWorks works;
    works.reserve(desc.material_override.size());

    for(const auto& matDesc: desc.material_override){
        if(!matDesc.baseColor.empty()){
            auto [kind, path] = splitSchemeAndPath(matDesc.baseColor);
            if(kind != SchemeKind::Unknown){
                WorkItem work{
                    .entityName = entityName,
                    .kind = kind,
                    .id = matDesc.baseColor,
                    .path = path,
                };
                works.push_back(std::move(work));
            } else{
                AppWarn("unknown scheme in baseColor: {}",
                    matDesc.baseColor);
            }
        }
    }

    return works;
}

auto AssetLoader::collectShaderWork(
    const std::string& entityName, const MeshDescriptor& desc
)->ShaderWork{
    const auto& sd = desc.shader;
    auto [kind, path] = splitSchemeAndPath(sd.module_);
    return ShaderWork{
        .kind = kind,
        .id = sd.module_,
        .path = path,
    };
}

void AssetLoader::executeMeshFileWork(const MeshWork& item){
    const auto& bindTarget = item.entityName;

    const auto& meshFileID = item.id;
    auto meshFileIt = table.find(meshFileID);
    auto isMeshLoaded = meshFileIt != table.end();

    CookedMesh cookedMesh;

    if(isMeshLoaded){
        bindMeshToEntity(bindTarget, meshFileIt->second);
    } else{
        cookedMesh = loadCookedMeshFor(item);
        auto newMeshUUID = loadSubmeshes(cookedMesh);

        remember(meshFileID, newMeshUUID);
        bindMeshToEntity(bindTarget, newMeshUUID);
    }

    // materialSet
    auto materialSetID = makeMaterialSetID(meshFileID, item.material_override);
    auto materialSetIt = table.find(materialSetID);
    auto isMaterialSetLoaded = materialSetIt != table.end();

    if(isMaterialSetLoaded){
        bindMaterialSetToEntity(bindTarget, materialSetIt->second);
    } else{
        auto newMaterialSetUUID = loadMaterialSet(
            meshFileID, cookedMesh, item.material_override);

        remember(materialSetID, newMaterialSetUUID);
        bindMaterialSetToEntity(bindTarget, newMaterialSetUUID);
    }

    auto shaderID = makeShaderID(item.shader);
    auto shaderIt = table.find(shaderID);
    auto isShaderLoaded = shaderIt != table.end();

    if(isShaderLoaded){
        bindShaderToEntity(bindTarget, shaderIt->second);
    } else{
        auto newShaderUUID = loadShader(item.shader);

        remember(shaderID, newShaderUUID);
        bindShaderToEntity(bindTarget, newShaderUUID);
    }
}

void AssetLoader::executeMeshEmbeddedWork(const MeshWork& item){
    const auto& bindTarget = item.entityName;

    const auto& meshEmbeddedID = item.id;
    auto meshEmbeddedIt = table.find(meshEmbeddedID);
    auto isMeshLoaded = meshEmbeddedIt != table.end();

    if(isMeshLoaded){
        bindMeshToEntity(bindTarget, meshEmbeddedIt->second);
    } else{
        auto fittedMesh = loadEmbeddedMesh(item.path);
        auto newMeshUUID = loadSubmeshes(fittedMesh);

        remember(meshEmbeddedID, newMeshUUID);
        bindMeshToEntity(bindTarget, newMeshUUID);
    }

    // materialSet
    auto materialSetID = makeMaterialSetID(
        meshEmbeddedID, item.material_override);
    auto materialSetIt = table.find(materialSetID);
    auto isMaterialSetLoaded = materialSetIt != table.end();

    if(isMaterialSetLoaded){
        bindMaterialSetToEntity(bindTarget, materialSetIt->second);
    } else{
        auto newMaterialSetUUID = loadMaterialSet(
            meshEmbeddedID, item.material_override);

        remember(materialSetID, newMaterialSetUUID);
        bindMaterialSetToEntity(bindTarget, newMaterialSetUUID);
    }

    auto shaderID = makeShaderID(item.shader);
    auto shaderIt = table.find(shaderID);
    auto isShaderLoaded = shaderIt != table.end();

    if(isShaderLoaded){
        bindShaderToEntity(bindTarget, shaderIt->second);
    } else{
        auto newShaderUUID = loadShader(item.shader);

        remember(shaderID, newShaderUUID);
        bindShaderToEntity(bindTarget, newShaderUUID);
    }
}

bool AssetLoader::bindMeshToEntity(const std::string& entityName, UUID uuid){
    auto entityMeshID = std::format("{}:mesh", entityName);
    auto [it, bindSuccess] = table.try_emplace(entityMeshID, uuid);

    return bindSuccess;
}

bool AssetLoader::bindMaterialSetToEntity(const std::string& entityName, UUID uuid){
    auto entityMaterialSetID = std::format("{}:materialSet", entityName);
    auto [it, bindSuccess] = table.try_emplace(entityMaterialSetID, uuid);

    return bindSuccess;
}

bool AssetLoader::bindShaderToEntity(const std::string& entityName, UUID uuid){
    auto entityShaderID = std::format("{}:shader", entityName);
    auto [it, bindSuccess] = table.try_emplace(entityShaderID, uuid);

    return bindSuccess;
}

CookedMesh AssetLoader::loadCookedMeshFor(const MeshWork& item){
    CookedMesh cooked = loadCookedOrImport(item.path);
    return cooked;
}

auto AssetLoader::loadSubmeshes(
    const CookedMesh& cooked
)->UUID{
    Mesh mesh;
    mesh.reserve(cooked.submeshInfoTable.size());

    for(const auto& submeshInfo: cooked.submeshInfoTable){
        std::span<const Vertex> vertices(
            cooked.vertices.cbegin() + submeshInfo.verticesSectionIndex,
            submeshInfo.vertexCount
        );
        std::span<const uint32_t> indices(
            cooked.indices.begin() + submeshInfo.indicesSectionIndex,
            submeshInfo.indexCount
        );

        auto submeshHandle = submeshManager.emplace(
            issueID(), renderContext, vertices, indices
        );
        mesh.push_back(std::move(submeshHandle));
    }

    auto newMeshUUID = issueID();
    meshTable.try_emplace(
        newMeshUUID, mesh);
    return newMeshUUID;
}

auto AssetLoader::loadSubmeshes(
    const EmbeddedMesh& fitted
)->UUID{
    Mesh mesh(1);

    std::span<const Vertex> vertices = fitted.vertices;
    std::span<const uint32_t> indices = fitted.indices;

    mesh[0] = submeshManager.emplace(
        issueID(), renderContext, vertices, indices
    );

    auto newMeshUUID = issueID();
    meshTable.try_emplace(
        newMeshUUID, mesh);
    return newMeshUUID;
}

auto AssetLoader::loadMaterialSet(
    const std::string& meshFileName,
    const CookedMesh& cooked,
    const MaterialDescriptors& descs
)->UUID{
    MaterialSet materialSet;
    materialSet.reserve(cooked.submeshInfoTable.size());

    std::unordered_map<std::string, Index> slotNameToIndex;
    for(Index i=0; i<cooked.materialInfoTable.size(); ++i)
        slotNameToIndex.try_emplace(cooked.materialNameTable[i], i);
        // TODO. add material slot name table
        // materialSlotToIndex.try_emplace(cooked.materialSlotTable[i], i);

    // load overrided material first
    for(const auto& desc: descs){
        auto slotIt = slotNameToIndex.find(desc.targetSlot);
        if(slotIt == slotNameToIndex.end()){
            AppWarn("Specified slot not exist: {}", desc.targetSlot);
            continue;
        }

        auto materialIt = table.find(desc.baseColor);
        auto isMaterialLoaded = materialIt != table.end();

        if(isMaterialLoaded){
            materialSet[slotIt->second] = shaderManager
                .getHandle(materialIt->second);
        } else{
            auto newMaterialUUID = issueID();

            materialSet[slotIt->second] = shaderManager
                .emplace(newMaterialUUID,
                    renderContext, desc.baseColor);
            remember(desc.baseColor, newMaterialUUID);
        }

        // target slot overrided
        slotNameToIndex.erase(slotIt);
    }

    // load material not in material_override slot next
    for(const auto& [slotName, i]: slotNameToIndex){
        auto materialID = std::format("{}:material{}",
            meshFileName, i);

        auto materialIt = table.find(materialID);
        auto isMaterialLoaded = materialIt != table.end();

        if(isMaterialLoaded){
            materialSet[i] = materialManager
                .getHandle(materialIt->second);
        } else{
            auto newMaterialUUID = issueID();

            const auto& textureInfo = cooked.textureInfoTable[
                cooked.materialInfoTable[i].textureInfoTableIndex + 0];
            std::span<const uint8_t> pixels(
                cooked.pixels.begin() + textureInfo.pixelSectionIndex,
                textureInfo.pixelCount
            );

            materialSet[i] = materialManager
                .emplace(newMaterialUUID, renderContext,
                    pixels, textureInfo.width, textureInfo.height);
            remember(materialID, newMaterialUUID);
        }
    }

    auto newMaterialSetUUID = issueID();
    materialSetTable.try_emplace(
        newMaterialSetUUID, materialSet);
    return newMaterialSetUUID;
}

auto AssetLoader::loadMaterialSet(
    const std::string& meshFileName,
    const MaterialDescriptors& descs
)->UUID{
    MaterialSet materialSet;
    materialSet.resize(descs.size());

    std::unordered_map<std::string, uint32_t> materialSlotToIndex;
    materialSlotToIndex.try_emplace("*", 0);

    for(const auto& desc: descs){
        auto materialSetIndex = materialSlotToIndex.at(desc.targetSlot);

        auto materialIt = table.find(desc.baseColor);
        auto isMaterialLoaded = materialIt != table.end();

        if(isMaterialLoaded){
            materialSet[materialSetIndex] = materialManager
                .getHandle(materialIt->second);
        } else{
            auto newMaterialUUID = issueID();

            auto [matScheme, matPath] = splitSchemeAndPath(desc.baseColor);
            materialSet[materialSetIndex] = materialManager
                .emplace(newMaterialUUID,
                    renderContext, matPath);
            remember(desc.baseColor, newMaterialUUID);
        }
    }

    auto newMaterialSetUUID = issueID();
    materialSetTable.try_emplace(
        newMaterialSetUUID, materialSet);
    return newMaterialSetUUID;
}

auto AssetLoader::loadShader(
    const ShaderDescriptor& desc
)->UUID{
    auto shaderID = makeShaderID(desc);
    auto shaderIt = table.find(shaderID);
    auto isShaderLoaded = shaderIt != table.end();

    if(isShaderLoaded)
        return shaderIt->second;

    auto newShaderUUID = issueID();
    remember(shaderID, newShaderUUID);
    (void)shaderManager.emplace(newShaderUUID,
        renderContext, desc.vsFunc, desc.fsFunc);

    return newShaderUUID;
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


