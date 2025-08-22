#include <limits>
#include "core/math/type.hpp"
#include "engine/resource_manager.hpp"
#include "engine/asset/scene_loader.hpp"
#include "engine/asset/scene_parser.hpp"
#include "../../game/component.hpp"
#include "../../game/entity_registry.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Asset;

namespace{
    EntityID invalidEntityID(){
        return std::numeric_limits<EntityID>::max();
    }

    auto convert(const TransformDescriptor& desc){    
        return Game::Transform{
            .entity = invalidEntityID(),
            .isActive = true,
            .position = desc.position,
            .rotation = desc.rotation,
            .scale = desc.scale
        };
    }

    auto convert(const MeshDescriptor& desc,
        const std::string& name,
        const ResolveTable& table,
        const ShaderManager& shaderManager
    ){
        auto meshUUID = table.at(std::format("{}:mesh", name));
        auto materialSetUUID = table.at(std::format("{}:materialSet", name));

        return Game::Mesh{
            .entity = invalidEntityID(),
            .isActive = true,
            .alpha = 1.0,
            .mesh = meshUUID,
            .materialSet = materialSetUUID,
            .shaderHandle = shaderManager.getHandle(
                table.at(desc.shader.module_)
            )
        };
    }

    auto convert(const RigidbodyDescriptor& desc){
        return Game::Rigidbody{
            .entity = invalidEntityID(),
            .isActive = true,
            .velocity = desc.velocity,
            .useGravity = desc.useGravity,
            .mass = static_cast<float>(desc.mass)
        };
    }

    auto convert(const ColliderMaterialDescriptor& desc){
        return Game::PhysicsMaterial{
            .bounciness = static_cast<float>(desc.bounciness),
            .friction = static_cast<float>(desc.friction),
        };
    }
    auto convert(const SphereColliderDescriptor& desc){
        return Game::SphereCollider{
            .entity = invalidEntityID(),
            .isActive = true,
            .position = desc.position,
            .radius = static_cast<float>(desc.radius),
            .material = convert(desc.material)
        };
    }
    auto convert(const BoxColliderDescriptor& desc){
        return Game::BoxCollider{
            .entity = invalidEntityID(),
            .isActive = true,
            .position = desc.position,
            .rotation = desc.rotation,
            .scale = desc.scale
        };
    }

    auto convert(const CameraDescriptor& desc){
        return Game::Camera{
            .entity = invalidEntityID(),
            .isActive = true,
            .type = cameraType(desc.type),
            .fov = static_cast<float>(desc.fov),
            .nearPlane = static_cast<float>(desc.nearPlane),
            .farPlane = static_cast<float>(desc.farPlane),
            .projection = projection(desc.projection)
        };
    }
    auto convert(const PlayerDescriptor&){
        return Game::Player{
            .entity = invalidEntityID(),
            .isActive = true
        };
    }
    auto convert(const EditorDescriptor&){
        return Game::Editor{
            .entity = invalidEntityID(),
            .isActive = true
        };
    }
}

SceneLoader::SceneLoader(
    Game::EntityRegistry& registry,
    ShaderManager& shaderManager
):registry(registry), shaderManager(shaderManager){}

void SceneLoader::loadScene(const SceneDescriptor& desc,
    const ResolveTable& table
){
    for(const auto& entity: desc.entities){
        std::optional<Game::Transform> transform = std::nullopt;
        std::optional<Game::Mesh> mesh = std::nullopt;
        std::optional<Game::Rigidbody> rigidbody = std::nullopt;
        std::optional<Game::SphereCollider> sphereCollider = std::nullopt;
        std::optional<Game::BoxCollider> boxCollider = std::nullopt;
        std::optional<Game::Camera> camera = std::nullopt;
        std::optional<Game::Player> player = std::nullopt;
        std::optional<Game::Editor> editor = std::nullopt;

        AppDebug("load entity {}", entity.name);

        if(entity.mask.test((size_t)ComponentKind::Transform)){
            if(entity.transformIndex == INVALID){
                AppWarn("Component integrity Broken");
                continue;
            }
            transform = convert(desc.transforms[entity.transformIndex]);
        }
        if(entity.mask.test((size_t)ComponentKind::Mesh)){
            if(entity.meshIndex == INVALID){
                AppWarn("Component integrity Broken");
                continue;
            }
            mesh = convert(desc.meshes[entity.meshIndex],
                entity.name, table, shaderManager);
        }
        if(entity.mask.test((size_t)ComponentKind::Rigidbody)){
            if(entity.rigidbodyIndex == INVALID){
                AppWarn("Component integrity Broken");
                continue;
            }
            rigidbody = convert(desc.rigidbodies[entity.rigidbodyIndex]);
        }
        if(entity.mask.test((size_t)ComponentKind::SphereCollider)){
            if(entity.sphereColliderIndex == INVALID){
                AppWarn("Component integrity Broken");
                continue;
            }
            sphereCollider = convert(desc.sphereColliders[entity.sphereColliderIndex]);
        }
        if(entity.mask.test((size_t)ComponentKind::BoxCollider)){
            if(entity.boxColliderIndex == INVALID){
                AppWarn("Component integrity Broken");
                continue;
            }
            boxCollider = convert(desc.boxColliders[entity.boxColliderIndex]);
        }
        if(entity.mask.test((size_t)ComponentKind::Camera)){
            if(entity.cameraIndex == INVALID){
                AppWarn("Component integrity Broken");
                continue;
            }
            camera = convert(desc.cameras[entity.cameraIndex]);
        }
        if(entity.mask.test((size_t)ComponentKind::Player)){
            if(entity.playerIndex == INVALID){
                AppWarn("Component integrity Broken");
                continue;
            }
            player = convert(desc.players[entity.playerIndex]);
        }
        if(entity.mask.test((size_t)ComponentKind::Editor)){
            if(entity.editorIndex == INVALID){
                AppWarn("Component integrity Broken");
                continue;
            }
            editor = convert(desc.editors[entity.editorIndex]);
        }

        registry.createEntity(transform, mesh, rigidbody,
            sphereCollider, boxCollider, camera, player, editor);
    }
}