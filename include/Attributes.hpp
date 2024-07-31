#pragma once

#include <vector>
#include <gsl-lite/gsl-lite.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Math.hpp"

namespace ModernBoy{
    struct Transform{
        pos position{0.0f};
        glm::quat quaternion{0.0f, 0.0f, 0.0f, 1.0f};
        glm::vec3 scale{1.0f};

        pos transform(const pos& vertexPos);
    };

    struct Material{
        fRGB ambient=fDUNE;
        fRGB diffuse=fBLUE;
        fRGB specular=fWHITE;
        float shininess=10.0f;

        float reflection=0.0f;
        float transparency=0.0f;
    };

    struct Polygon{
        gsl::index index[3];
    };

    struct Mesh{
        std::vector<glm::vec3> verticies;
        std::vector<glm::vec3> normals;
        std::vector<Polygon> polygons;
    };
}
