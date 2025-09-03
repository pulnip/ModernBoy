#include "ray_service.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

void RayService::write(const Ray& ray){
    std::lock_guard lock(mtx);
    rays.push_back(ray);
}

std::vector<Ray> RayService::drainRays(){
    std::vector<Ray> out;
    {
        std::lock_guard lock(mtx);
        out.swap(rays);
    }
    return out;
}
