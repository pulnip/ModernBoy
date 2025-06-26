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
InputComponent ModernBoy::dangled(Input::InputMap&& map){
    return {std::numeric_limits<EntityID>::max(),
        true, map };
}
