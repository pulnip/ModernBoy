#ifndef MODERNBOY_ASSET_SCENEPARSER_HPP
#define MODERNBOY_ASSET_SCENEPARSER_HPP

#include <bitset>
#include <limits>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "core/math/type.hpp"

namespace ModernBoy::Asset
{
    // common values
    struct SourceLocation{
        size_t line, column; 
    };

    struct VNull{
        SourceLocation location;
    };
    struct VBool{
        bool v;
        SourceLocation location;
    };
    struct VInt{
        int64_t v;
        SourceLocation location;
    };
    struct VFloat{
        double v;
        SourceLocation location;
    };
    struct VString{
        std::string v;
        SourceLocation location;
    };
    struct VArray{
        std::vector<size_t> elements;
        SourceLocation location;
    };
    struct VTable{
        std::unordered_map<std::string, size_t> fields;
        SourceLocation location;
    };

    using VNode = std::variant<
        VNull, VBool, VInt, VFloat,
        VString, VArray, VTable>;

    struct ValueArena{
        std::vector<VNode> nodes;
        inline size_t emplace(VNode n){
            nodes.push_back(std::move(n));
            return nodes.size() - 1;
        }
    };

    // parse result
    struct TempEntity{
        std::string name;
        size_t componentsTableIndex = std::numeric_limits<size_t>::max();
        SourceLocation location;
    };
    struct TempScene{
        ValueArena arena;
        std::vector<TempEntity> entities;
    };

    // SoA Intermediate Representation
    enum class ComponentKind: uint8_t{
        Transform = 0,
        Mesh = 1,
        Rigidbody = 2,
        BoxCollider = 3,
        SphereCollider = 4,
        Camera = 5,
        Player = 6,
        Editor = 7,
        Count = 8
    };
    constexpr auto INVALID = std::numeric_limits<uint32_t>::max();

    struct TransformDescriptor{
        DEFINE_TRANSFORM;
    };
    struct MaterialDescriptor{
        std::string baseColor;
    };
    struct ShaderDescriptor{
        std::string module_;
        std::string vsFunc;
        std::string fsFunc;
    };
    struct MeshDescriptor{
        std::string id;
        MaterialDescriptor material_override;
        ShaderDescriptor shader;
    };
    struct RigidbodyDescriptor{
        Vec3 velocity;
        bool useGravity;
        double mass;
    };
    struct ColliderMaterialDescriptor{
        double bounciness;
        double friction;
    };
    struct BoxColliderDescriptor{
        DEFINE_TRANSFORM;
        ColliderMaterialDescriptor material;
    };
    struct SphereColliderDescriptor{
        Vec3 position;
        double radius;
        ColliderMaterialDescriptor material;
    };
    struct CameraDescriptor{
        std::string type;
        double fov;
        double nearPlane, farPlane;
        std::string projection;
    };
    struct PlayerDescriptor{};
    struct EditorDescriptor{};

    struct Entity{
        std::string name;
        std::bitset<(size_t)8> mask;
        uint32_t transformIndex = INVALID;
        uint32_t meshIndex = INVALID;
        uint32_t rigidbodyIndex = INVALID;
        uint32_t boxColliderIndex = INVALID;
        uint32_t sphereColliderIndex = INVALID;
        uint32_t cameraIndex = INVALID;
        uint32_t playerIndex = INVALID;
        uint32_t editorIndex = INVALID;
    };
    struct SceneDescriptor{
        // SoA
        std::vector<TransformDescriptor> transforms;
        std::vector<MeshDescriptor> meshes;
        std::vector<RigidbodyDescriptor> rigidbodies;
        std::vector<BoxColliderDescriptor> boxColliders;
        std::vector<SphereColliderDescriptor> sphereColliders;
        std::vector<CameraDescriptor> cameras;
        std::vector<PlayerDescriptor> players;
        std::vector<EditorDescriptor> editors;

        std::vector<Entity> entities;

        inline uint32_t pushTransform(const TransformDescriptor& desc){
            transforms.push_back(desc);
            return static_cast<uint32_t>(transforms.size() - 1);
        }
        inline uint32_t pushMesh(const MeshDescriptor& desc){
            meshes.push_back(desc);
            return static_cast<uint32_t>(meshes.size() - 1);
        }
        inline uint32_t pushRigidbody(const RigidbodyDescriptor& desc){
            rigidbodies.push_back(desc);
            return static_cast<uint32_t>(rigidbodies.size() - 1);
        }
        inline uint32_t pushBoxCollider(const BoxColliderDescriptor& desc){
            boxColliders.push_back(desc);
            return static_cast<uint32_t>(boxColliders.size() - 1);
        }
        inline uint32_t pushSphereCollider(const SphereColliderDescriptor& desc){
            sphereColliders.push_back(desc);
            return static_cast<uint32_t>(sphereColliders.size() - 1);
        }
        inline uint32_t pushCamera(const CameraDescriptor& desc){
            cameras.push_back(desc);
            return static_cast<uint32_t>(cameras.size() - 1);
        }
        inline uint32_t pushPlayer(const PlayerDescriptor& desc){
            players.push_back(desc);
            return static_cast<uint32_t>(players.size() - 1);
        }
        inline uint32_t pushEditor(const EditorDescriptor& desc){
            editors.push_back(desc);
            return static_cast<uint32_t>(editors.size() - 1);
        }
    };

    // bind/freeze plan
    struct BindError{
        std::string msg;
        SourceLocation location;
    };
    struct PlannedTransform{
        TransformDescriptor desc;
        // bind transform to entity by index
        size_t entityIndex = std::numeric_limits<size_t>::max();
        SourceLocation location{};
    };
    struct PlannedMesh{
        MeshDescriptor desc;
        size_t entityIndex = std::numeric_limits<size_t>::max();
        SourceLocation location{};
    };
    struct PlannedRigidbody{
        RigidbodyDescriptor desc;
        size_t entityIndex = std::numeric_limits<size_t>::max();
        SourceLocation location{};
    };
    struct PlannedBoxCollider{
        BoxColliderDescriptor desc;
        size_t entityIndex = std::numeric_limits<size_t>::max();
        SourceLocation location{};
    };
    struct PlannedSphereCollider{
        SphereColliderDescriptor desc;
        size_t entityIndex = std::numeric_limits<size_t>::max();
        SourceLocation location{};
    };
    struct PlannedCamera{
        CameraDescriptor desc;
        size_t entityIndex = std::numeric_limits<size_t>::max();
        SourceLocation location{};
    };
    struct PlannedPlayer{
        PlayerDescriptor desc;
        size_t entityIndex = std::numeric_limits<size_t>::max();
        SourceLocation location{};
    };
    struct PlannedEditor{
        EditorDescriptor desc;
        size_t entityIndex = std::numeric_limits<size_t>::max();
        SourceLocation location{};
    };

    struct BindPlan{
        std::vector<PlannedTransform> transforms;
        std::vector<PlannedMesh> meshes;
        std::vector<PlannedRigidbody> rigidbodies;
        std::vector<PlannedBoxCollider> boxColliders;
        std::vector<PlannedSphereCollider> sphereColliders;
        std::vector<PlannedCamera> cameras;
        std::vector<PlannedPlayer> players;
        std::vector<PlannedEditor> editors;
        std::vector<BindError> errors;
    };

    // component binder interface
    class IComponentBinder{
    public:
        virtual ~IComponentBinder() = default;
        virtual void validateAndPlan(const ValueArena&,
            const VTable&, size_t entityIndex, BindPlan&)=0;
    };

    using BinderRegistry = std::unordered_map<std::string, std::unique_ptr<IComponentBinder>>;

    TempScene parseSceneFromFile(const std::string& sceneFile);
    TempScene parseSceneFromString(std::string_view tomlText);

    // Build (Bind + Freeze): TempScene → SceneDescriptor using registered binders
    SceneDescriptor buildScene(const TempScene& temp, const BinderRegistry& registry);

    // Convenience: default registry with built-in binders
    BinderRegistry makeDefaultBinderRegistry();
} // namespace ModernBoy::Asset

#endif // MODERNBOY_ASSET_SCENEPARSER_HPP