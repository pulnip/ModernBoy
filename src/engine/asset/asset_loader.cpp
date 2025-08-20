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

Asset::AssetLoader::AssetLoader(SubmeshManager& submeshManager,
    MeshManager& meshManager, TextureManager& texManager,
    MaterialTable& materialTable, MaterialSetTable& materialSetTable,
    ShaderManager& shaderManager, NativePtr renderContext)
:submeshManager(submeshManager), meshManager(meshManager), textureManager(texManager),
materialTable(materialTable), materialSetTable(materialSetTable),
shaderManager(shaderManager), renderContext(renderContext){}

void Asset::AssetLoader::load(const SceneDescriptor& desc){
    load(desc.meshes);
}

void Asset::AssetLoader::load(const std::vector<MeshDescriptor>& meshes){
    std::vector<MeshWork> meshWorks;
    std::vector<WorkItem> materialWorks, shaderWorks;

    for(const auto& meshDesc: meshes)
        collectWorkItems(meshDesc, meshWorks, materialWorks, shaderWorks);

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

void Asset::AssetLoader::collectWorkItems(const MeshDescriptor& meshDesc,
    std::vector<MeshWork>& meshWorks, std::vector<WorkItem>& matWorks,
    std::vector<WorkItem>& shaderWorks
){
    {
        auto [kind, path] = splitSchemeAndPath(meshDesc.id);
        MeshWork meshWork{
            .kind = kind,
            .id = meshDesc.id,
            .path = path,
            .uuid = issueID(),
            .material_override = meshDesc.material_override
        };
        remember(meshWork.id, meshWork.uuid);
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
                        .kind = kind,
                        .id = matDesc.baseColor,
                        .path = path,
                        .uuid = issueID()
                    };
                    remember(matWork.id, matWork.uuid);
                    matWorks.push_back(std::move(matWork));
                } else{
                    AppWarn("unknown seheme in baseColor: {}",
                        matDesc.baseColor);
                }
            }
        }
    }

    {
        if(meshDesc.shader){
            const auto& sd = *meshDesc.shader;
            auto [kind, path] = splitSchemeAndPath(sd.module_);
            if(kind != SchemeKind::Unknown){
                WorkItem shaderWork{
                    .kind = kind,
                    .id = sd.module_,
                    .path = path,
                    .uuid = issueID()
                };
                remember(sd.module_, shaderWork.uuid);
                shaderWorks.push_back(std::move(shaderWork));
            }
        }
    }
}

// Process a "file" scheme on mesh id
void Asset::AssetLoader::processMeshFile(const MeshWork& item
){
    std::unordered_map<std::string, uint32_t> materialNameToIndex;
    CookedMesh cooked = loadCookedOrImport(item.path);

    // submesh
    std::vector<SubmeshHandle> mesh(cooked.submeshInfoTable.size());
    for(Index i=0; i<cooked.submeshInfoTable.size(); ++i){
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

    auto meshHandle = meshManager.emplace(
        item.uuid, mesh);
    (void) meshHandle;

    for(Index i=0; i<cooked.materialInfoTable.size(); ++i)
        materialNameToIndex.try_emplace(cooked.materialNameTable[i], i);
    auto useDefaultMaterialMap = materialNameToIndex;
    for(const auto& matDesc: item.material_override){
        useDefaultMaterialMap.erase(matDesc.targetSlot);
    }

    // material
    std::vector<UUID> materialSet;
    for(const auto& [_, i]: useDefaultMaterialMap){
        const auto& materialInfo = cooked.materialInfoTable[i];
        const auto& materialName = cooked.materialNameTable[i];

        // check material already loaded
        auto it = table.find(std::format("{}:{}", item.id, materialName));
        if(it != table.end()){
            const auto [_2, uuid] = *it;
            materialSet[i] = uuid;
            continue;
        }

        // not loaded, so loading process start
        std::vector<TextureHandle> material(materialInfo.textureCount);
        // texture
        for(Index j=0; j<materialInfo.textureCount; ++j){
            const auto& textureInfo = cooked.textureInfoTable[
                materialInfo.textureInfoTableIndex + j];

            std::span<uint8_t> pixels(
                cooked.pixels.begin() + textureInfo.pixelSectionIndex,
                textureInfo.pixelCount
            );
            auto texID = issueID();
            remember(std::format("{}:{}:{}", item.id, materialName, i), texID);

            material[j] = textureManager.emplace(
                texID, renderContext, pixels,
                textureInfo.width, textureInfo.height
            );
        }

        auto matID = issueID();
        remember(std::format("{}:{}", item.id, materialName), matID);
        materialToUUID.try_emplace(material, matID);

        materialTable.try_emplace(matID, material);
        materialSet[i] = matID;
    }
    // override material
    for(const auto& matDesc: item.material_override){
        auto it = materialNameToIndex.find(matDesc.targetSlot);
        if(it == materialNameToIndex.end()){
            AppWarn("Target slot '{}' not exist on mesh:",
                matDesc.targetSlot, item.id);
            continue;
        }

        auto [_, override_index] = *it;
        // check material already loaded
        auto tit = table.find(matDesc.baseColor);
        if(tit != table.end()){
            const auto& [_2, uuid] = *tit;
            
            materialSet[override_index] = uuid;
        }

        auto texID = issueID();
        auto handle = textureManager.emplace(
            texID, renderContext, matDesc.baseColor
        );
        std::vector<TextureHandle> material;
        material.push_back(handle);

        auto matID = issueID();
        remember(matDesc.baseColor, matID);
        materialTable.try_emplace(matID, material);
        materialSet[override_index] = matID;
    }

    auto matSetID = issueID();
    remember(std::format("{}:materialSet", item.id), matSetID);
    materialSetTable.try_emplace(matSetID, materialSet);
}

CookedMesh Asset::AssetLoader::loadCookedOrImport(
    const std::string& path
){
    const auto ext = getExtension(path);
    if(ext == "mbmesh")
        return loadMeshFile(path);
    else
        return importMeshFile(path);
}

std::vector<TextureHandle> Asset::AssetLoader::createTexturesFromMesh(
    const CookedMesh& mesh, const std::string& baseID
){
    std::vector<TextureHandle> textureHandles;
    textureHandles.reserve(mesh.textureInfoTable.size());

    for(Index i=0; i<mesh.textureInfoTable.size(); ++i){
        const auto& ti   = mesh.textureInfoTable[i];
        const size_t off = ti.pixelSectionIndex;
        const size_t sz  = ti.pixelCount;

        if(off > mesh.pixels.size() || sz > mesh.pixels.size() - off){
            AppWarn("texture[{}] OOR: base={}, size={}, total={}", i, off, sz, mesh.pixels.size());
            continue;
        }

        auto texID = issueID();
        table.try_emplace(std::format("{}:texture{}:{}",
            baseID, i, usageToStr(ti.usage)), texID);

        const auto pixelBase = mesh.textureInfoTable[i].pixelSectionIndex;
        const auto w = mesh.textureInfoTable[i].width;
        const auto h = mesh.textureInfoTable[i].height;
        if(pixelBase + static_cast<size_t>(w) * h > mesh.pixels.size()) {
            AppWarn("texture pixel span out of range: idx={}, size={}",
                pixelBase, mesh.pixels.size());
            return {};
        }

        auto handle = textureManager.emplace(
            texID,
            renderContext,
            std::span<const uint8_t>{
                &mesh.pixels[pixelBase],
                static_cast<size_t>(w) * h },
            w, h
        );
        textureHandles.push_back(handle);
    }

    return textureHandles;
}

void Asset::AssetLoader::createMaterialFromTextures(
    const std::vector<TextureHandle>& textures
){
    materialToUUID.try_emplace(textures, issueID());
}

// Process an "embedded" scheme on mesh id
void Asset::AssetLoader::processMeshEmbedded(const MeshWork& item){
    auto handle = meshManager.emplace(
        item.uuid, renderContext, item.path);
    (void)handle;
}

void Asset::AssetLoader::processMaterial(const WorkItem& item){
    auto handle = textureManager.emplace(
        item.uuid, renderContext, item.path);
    (void)handle;
}

void Asset::AssetLoader::processShader(const WorkItem& item){
    auto handle = shaderManager.emplace(
        item.uuid, renderContext, item.path);
    (void)handle;
}


