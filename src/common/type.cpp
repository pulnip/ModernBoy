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

Vec2 ModernBoy::operator+(Vec2 lhs, Vec2 rhs){
    return {{lhs.x+rhs.x, lhs.y+rhs.y}};
}
Vec2 ModernBoy::operator-(Vec2 lhs, Vec2 rhs){
    return {{lhs.x-rhs.x, lhs.y-rhs.y}};
}
Vec2 ModernBoy::operator-(Vec2 v){
    return {{-v.x, -v.y}};
}
Vec2 ModernBoy::operator/(Vec2 v, float f){
    return {{v.x/f, v.y/f}};
}

bool ModernBoy::operator==(Vec2 lhs, Vec2 rhs){
    return (lhs.x==rhs.x) && (lhs.y==rhs.y);
}

Vec3 ModernBoy::operator/(Vec3 v, float f){
    return {{v.x/f, v.y/f, v.z/f}};
}
bool ModernBoy::operator==(Vec3 lhs, Vec3 rhs){
    return lhs.x==rhs.x && lhs.y==rhs.y && lhs.z==rhs.z;
}

bool ModernBoy::operator==(Vec4 lhs, Vec4 rhs){
    return lhs.x==rhs.x && lhs.y==rhs.y && lhs.z==rhs.z && lhs.w==rhs.w;
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

ResourceHandle ModernBoy::invalidResourceHandle(){
    return {ResourceType::INVALID,
        std::numeric_limits<size_t>::max()};
}
bool ModernBoy::isValid(ResourceHandle handle){
    return handle.index != std::numeric_limits<size_t>::max();
}
bool ModernBoy::operator==(const ResourceHandle& lhs, const ResourceHandle& rhs){
    return lhs.index==rhs.index;
}
bool ModernBoy::operator!=(const ResourceHandle& lhs, const ResourceHandle& rhs){
    return !(lhs==rhs);
}
bool ModernBoy::operator<(const ResourceHandle& lhs, const ResourceHandle& rhs){
    return lhs.index<rhs.index;
}
