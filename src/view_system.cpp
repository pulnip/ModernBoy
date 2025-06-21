#include "view_system.hpp"

using namespace ModernBoy;

ViewTasks CameraComponent::getTasks() const{
    ViewTasks tasks;

    if(enabled){
        ViewTask newTask{transformHandle, cameraHandle};
        tasks.emplace_back(std::move(newTask));
    }

    return tasks;
}