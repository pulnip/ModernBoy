#include "Attributes.hpp"

using namespace glm;
using namespace ModernBoy;

WorldPos Transform::transform(const WorldPos& vertexPos){
    const auto translation=translate(mat4(1.0f), position);
    const auto rotation=toMat4(quaternion);
    const auto _scale=glm::scale(mat4(1.0f), scale);

    const auto transform=translation*rotation*_scale;

    return transform*vec4{vertexPos, 1.0f};
}
