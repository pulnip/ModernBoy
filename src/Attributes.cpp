#include "Attributes.hpp"

using namespace glm;
using namespace ModernBoy;

pos Transform::transform(const pos& vertexPos){
    const auto rotation=toMat3(quaternion);
    const mat3 scaling{vec3(scale.x), vec3(scale.y), vec3(scale.z)};

    return rotation*scaling*static_cast<vec3>(vertexPos) + position;
}
