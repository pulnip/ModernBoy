#include <limits>
#include <unordered_map>
#include "common/type.hpp"
#include "common/helper.hpp"

using namespace ModernBoy;

Vec3 ModernBoy::zeros(){
    return {.v={0.0f, 0.0f, 0.0f}};
}
Vec3 ModernBoy::ones(){
    return {.v={1.0f, 1.0f, 1.0f}};
}
Vec4 ModernBoy::unitQuat(){
    return {.v={0.0f, 0.0f, 0.0f, 1.0f}};
}

Transform ModernBoy::identity(){
    return{ zeros(), unitQuat(), ones() };
}

static std::unordered_map<std::string, CameraType>
text2camera = {
    {"MAINCAMERA", CameraType::MainCamera},
    { "SUBCAMERA",  CameraType::SubCamera},
};
CameraType ModernBoy::cameraType(const std::string& text){
    auto upper = toUpper(text);
        auto it = text2camera.find(upper);
    if (it == text2camera.end()){
        return CameraType::UNKNOWN;
    }
    return it->second;
}

static std::unordered_map<std::string, Projection>
text2projection = {
    {"PERSPECTIVE", Projection::PERSPECTIVE},
    {"ORTHOGRAPHIC",  Projection::ORTHOGRAPHIC},
};
Projection ModernBoy::projection(const std::string& text){
    auto upper = toUpper(text);
        auto it = text2projection.find(upper);
    if (it == text2projection.end()){
        return Projection::UNKNOWN;
    }
    return it->second;
}

Camera ModernBoy::defaultCamera(){
    return { CameraType::MainCamera,
        100.0f, 0.1f, 100.0f, Projection::PERSPECTIVE };
}

NaiveHandle ModernBoy::invalidHandle(){
    return {std::numeric_limits<size_t>::max(), 0};
}
bool ModernBoy::isValid(NaiveHandle handle){
    return handle.index != std::numeric_limits<size_t>::max();
}
bool ModernBoy::operator==(const NaiveHandle& lhs, const NaiveHandle& rhs){
    return lhs.index==rhs.index 
        & lhs.generation==rhs.generation;
}

ResourceHandle ModernBoy::invalidResourceHandle(){
    return {ResourceType::INVALID,
        std::numeric_limits<size_t>::max(), 0};
}
bool ModernBoy::isValid(ResourceHandle handle){
    return handle.index != std::numeric_limits<size_t>::max();
}
bool ModernBoy::operator==(const ResourceHandle& lhs, const ResourceHandle& rhs){
    return lhs.index==rhs.index 
        & lhs.generation==rhs.generation;
}
bool ModernBoy::operator!=(const ResourceHandle& lhs, const ResourceHandle& rhs){
    return !(lhs==rhs);
}
bool ModernBoy::operator<(const ResourceHandle& lhs, const ResourceHandle& rhs){
    return lhs.index<rhs.index;
}
