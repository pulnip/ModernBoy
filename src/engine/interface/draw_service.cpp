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

void DrawService::write(const MeshDrawCall& drawCall){
    std::lock_guard lock(drawCallMtx);
    drawCalls.push_back(drawCall);
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

std::vector<MeshDrawCall> DrawService::drainDrawCalls(){
    std::vector<MeshDrawCall> out;
    {
        std::lock_guard lock(drawCallMtx);
        sortDrawCalls();
        out.swap(drawCalls);
    }
    return out;
}

void DrawService::sortDrawCalls(){
    std::ranges::sort(drawCalls,
        [](const auto& lhs, const auto& rhs){
            return lhs.shaderHandle < rhs.shaderHandle ||
                lhs.materialHandle < rhs.materialHandle ||
                lhs.submeshHandle < rhs.submeshHandle;
        }
    );
}
