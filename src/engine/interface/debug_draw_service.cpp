#include "engine/interface/debug_draw_service.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Interface;

void DebugDrawService::write(const Line& line){
    std::lock_guard lock(lineMtx);
    lines.push_back(line);

}

void DebugDrawService::write(const Sphere& sphere){
    std::lock_guard lock(sphereMtx);
    spheres.push_back(sphere);
}

std::vector<Line> DebugDrawService::drainLines(){
    std::vector<Line> out;
    {
        std::lock_guard lock(lineMtx);
        out.swap(lines);
    }
    return out;
}

std::vector<Sphere> DebugDrawService::drainSpheres(){
    std::vector<Sphere> out;
    {
        std::lock_guard lock(lineMtx);
        out.swap(spheres);
    }
    return out;
}