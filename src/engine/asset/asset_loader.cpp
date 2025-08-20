#include <span>
#include "engine/asset/asset_loader.hpp"
#include "engine/engine.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Asset;

namespace{
    using namespace ModernBoy;
    using namespace ModernBoy::Asset;

    // "scheme:path" -> {scheme, path}
    std::pair<std::string, std::string> splitSchemeAndPath(
        const std::string& id
    ){
        auto colon_pos = id.find(':');
        if(colon_pos == std::string::npos)
            return {"", ""};
        return {
            id.substr(0, colon_pos),
            id.substr(colon_pos + 1)
        };
    }

    // Get file extension (without dot).
    std::string getExtension(const std::string& path){
        auto ext_pos = path.find_last_of('.');
        if( ext_pos == std::string::npos ||
            ext_pos + 1 >= path.size() )
            return "";
        return path.substr(ext_pos + 1);
    }
}

Asset::AssetLoader::AssetLoader(
    MeshManager& meshManager, TextureManager& texManager,
    ShaderManager& shaderManager, NativePtr renderContext)
:meshManager(meshManager), textureManager(texManager),
shaderManager(shaderManager), renderContext(renderContext){}

void Asset::AssetLoader::load(const SceneDescriptor& desc){
    load(desc.meshes);
}

void Asset::AssetLoader::load(const std::vector<MeshDescriptor>& meshes){
    std::vector<WorkItem> meshWork, textureWork, shaderWork;
    meshWork.reserve(meshes.size());
    textureWork.reserve(2 * meshes.size());
    shaderWork.reserve(meshes.size() / 2 + 1);

    // collect all WorkItem
    for(const auto& meshDesc: meshes){
        collectWorkItemFromDescriptor(meshDesc.id, meshWork);
        if(auto mo = meshDesc.material_override){
            for(const auto& md: *mo)
                collectWorkItemFromDescriptor(md.baseColor, textureWork);
        }
        if(auto so = meshDesc.shader){
            collectWorkItemFromDescriptor(so->module_, shaderWork);
        }
    }

    // Execute meshWork
    for(const auto& item: meshWork){
        switch(item.kind){
        case SchemeKind::File:
            processMeshFile(item);
            break;
        case SchemeKind::Embedded:
            processMeshEmbedded(item);
            break;
        default:
            AppWarn("id without scheme detected: {}", item.id);
            break;
        }
    }
    // Execute TextureWork
    for(const auto& item: textureWork){
        switch(item.kind){
        case SchemeKind::File:
            processTexture(item);
            break;
        case SchemeKind::Embedded:
            AppWarn("embedded texture not supported");
            break;
        default:
            AppWarn("id without scheme detected: {}", item.id);
            break;
        }
    }
    // Execute ShaderWork
    for(const auto& item: shaderWork){
        switch(item.kind){
        case SchemeKind::File:
            processShader(item);
            break;
        case SchemeKind::Embedded:
            AppWarn("embedded shader not supported");
            break;
        default:
            AppWarn("id without scheme detected: {}", item.id);
            break;
        }
    }
}

void Asset::AssetLoader::collectWorkItemFromDescriptor(
    const std::string& id, std::vector<WorkItem>& workItems
){
    // Skip if already loaded
    if(!table.contains(id)){
        // Parse scheme and path
        auto [scheme, path] = splitSchemeAndPath(id);
        if(!scheme.empty()){
            SchemeKind kind = SchemeKind::Unknown;
            if(scheme == "file")
                kind = SchemeKind::File;
            else if(scheme == "embedded")
                kind = SchemeKind::Embedded;

            // Reserve a new mesh ID and register it
            WorkItem item{
                .kind=kind,
                .id=id,
                .path=path,
                .uuid=issueID()
            };
            table.try_emplace(id, item.uuid);
            workItems.push_back(std::move(item));
        }
        else
            AppWarn("id without scheme detected: {}", id);
    }
}

// Process a "file" scheme on mesh id
void Asset::AssetLoader::processMeshFile(const WorkItem& item){
    CookedMesh mesh = loadCookedOrImport(item.path);

    auto meshHandle = meshManager.emplace(
        item.uuid, renderContext, mesh);
    (void) meshHandle;

    // Textures
    auto textureIDs = createTexturesFromMesh(mesh, item.id);

    // Materials
    createMaterialFromMesh(mesh, textureIDs);
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

// Process an "embedded" scheme on mesh id
void Asset::AssetLoader::processMeshEmbedded(const WorkItem& item){
    auto handle = meshManager.emplace(
        item.uuid, renderContext, item.path);
    (void)handle;
}

void Asset::AssetLoader::processTexture(const WorkItem& item){
    auto handle = textureManager.emplace(
        item.uuid, renderContext, item.path);
    (void)handle;
}

void Asset::AssetLoader::processShader(const WorkItem& item){
    auto handle = shaderManager.emplace(
        item.uuid, renderContext, item.path);
    (void)handle;
}

std::vector<UUID> Asset::AssetLoader::createTexturesFromMesh(
    const CookedMesh& mesh, const std::string& baseID
){
    std::vector<UUID> textureIDs;
    textureIDs.reserve(mesh.textureInfoTable.size());

    for(Index i=0; i<mesh.textureInfoTable.size(); ++i){
        auto texID = issueID();
        table.try_emplace(std::format("{}:texture{}",
            baseID, i), texID);

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
        (void)handle;

        textureIDs.push_back(texID);
    }

    return textureIDs;
}

void Asset::AssetLoader::createMaterialFromMesh(
    const CookedMesh& mesh, const std::vector<UUID>& texIDs\
){
    for(Index i=0; i<mesh.materialInfoTable.size(); ++i){
        const auto& material = mesh.materialInfoTable[i];

        std::vector<TextureHandle> textures;
        textures.reserve(material.textureCount);

        for(Index j=0; j<material.textureCount; ++j){
            auto base = material.textureInfoTableIndex;
            textures.push_back(
                textureManager.getHandle(texIDs[base + j])
            );
        }

        materialSets.emplace(textures);
    }
}
