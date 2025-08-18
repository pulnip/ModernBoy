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
        Count = 2
    };
    constexpr auto INVALID = std::numeric_limits<uint32_t>::max();

    struct ResourceReference{
        std::string schemePath;
    };
    struct TransformDescriptor{
        DEFINE_TRANSFORM;
    };
    struct MaterialDescriptor{
        ResourceReference baseColor;
    };
    struct ShaderDescriptor{
        ResourceReference module_;
        std::string vsFunc;
        std::string fsFunc;
    };
    struct MeshDescriptor{
        ResourceReference id;
        MaterialDescriptor material_override;
        ShaderDescriptor shader;
    };

    struct Entity{
        std::string name;
        std::bitset<(size_t)8> mask;
        uint32_t transformIndex = INVALID;
        uint32_t meshIndex = INVALID;
    };
    struct SceneDescriptor{
        // SoA
        std::vector<TransformDescriptor> transforms;
        std::vector<MeshDescriptor> meshes;

        std::vector<Entity> entities;

        inline uint32_t pushTransform(const TransformDescriptor& desc){
            transforms.push_back(desc);
            return static_cast<uint32_t>(transforms.size() - 1);
        }
        inline uint32_t pushMesh(const MeshDescriptor& desc){
            meshes.push_back(desc);
            return static_cast<uint32_t>(meshes.size() - 1);
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

    struct BindPlan{
        std::vector<PlannedTransform> transforms;
        std::vector<PlannedMesh> meshes;
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