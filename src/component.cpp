#include <limits>
#include <unordered_map>
#include "common/helper.hpp"
#include "component.hpp"

using namespace ModernBoy;

bool ModernBoy::subset(ArchetypeBit a, ArchetypeBit b){
    return (a & b) == a;
}

template<>
TransformComponent ModernBoy::dangled(Transform transform){
    return {std::numeric_limits<EntityID>::max(),
        true, transform };
}
template<>
CameraComponent ModernBoy::dangled(Camera camera, bool active){
    return {std::numeric_limits<EntityID>::max(),
        active, camera};
}
template<>
MeshComponent ModernBoy::dangled(Handle handle){
    return {std::numeric_limits<EntityID>::max(),
        true, handle};
}
template<>
InputComponent ModernBoy::dangled(){
    return {std::numeric_limits<EntityID>::max(),
        true, 0, {}};
}
