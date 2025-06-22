#include "raw_resource.hpp"

using namespace ModernBoy;

Vector3 ModernBoy::zeros(){ return {0.0f, 0.0f, 0.0f}; }
Vector3 ModernBoy::ones(){ return {1.0f, 1.0f, 1.0f}; }
Vector4 ModernBoy::unitQuaternion(){ return {0.0f, 0.0f, 0.0f, 1.0f}; }

ViewTasks CameraComponent::getTasks() const{
    ViewTasks tasks;

    if(enabled){
        ViewTask newTask{transformHandle, cameraHandle};
        tasks.emplace_back(std::move(newTask));
    }

    return tasks;
}
