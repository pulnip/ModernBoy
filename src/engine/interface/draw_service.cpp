#include <algorithm>
#include "engine/interface/draw_service.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Interface;

void DrawService::write(const Line& line){
    std::lock_guard lock(lineMtx);
    lines.push_back(line);
}

void DrawService::write(const Sphere& sphere){
    std::lock_guard lock(sphereMtx);
    spheres.push_back(sphere);
}

void DrawService::write(const MeshObject& obj){
    std::lock_guard lock(meshMtx);
    meshObjects.push_back(obj);
}

std::vector<Line> DrawService::drainLines(){
    std::vector<Line> out;
    {
        std::lock_guard lock(lineMtx);
        out.swap(lines);
    }
    return out;
}

std::vector<Sphere> DrawService::drainSpheres(){
    std::vector<Sphere> out;
    {
        std::lock_guard lock(sphereMtx);
        out.swap(spheres);
    }
    return out;
}

std::vector<MeshObject> DrawService::drainMeshObjects(){
    std::vector<MeshObject> out;
    {
        std::lock_guard lock(meshMtx);
        sortMeshObjects();
        out.swap(meshObjects);
    }
    return out;
}

void DrawService::sortMeshObjects(){
    std::ranges::sort(meshObjects,
        [](const auto& lhs, const auto& rhs){
            return lhs.shaderHandle < rhs.shaderHandle ||
                lhs.texHandle < rhs.texHandle ||
                lhs.meshHandle < rhs.meshHandle;
        }
    );
}
