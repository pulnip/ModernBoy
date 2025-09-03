#include "interface/view_service.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Interface;

void ViewService::write(const CameraObject& obj){
    std::lock_guard lock(mtx);
    cameraObjects.push_back(obj);
}

std::vector<CameraObject> ViewService::drainCameraObjects(){
    std::vector<CameraObject> out;
    {
        std::lock_guard lock(mtx);
        out.swap(cameraObjects);
    }
    return out;
}
