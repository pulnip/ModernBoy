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