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

float ModernBoy::dot(Vec2 lhs, Vec2 rhs){
    return lhs.x*rhs.x + lhs.y*rhs.y;
}
float ModernBoy::cross(Vec2 lhs, Vec2 rhs){
    return lhs.x*rhs.y - lhs.y*rhs.x;
}

Vec3 ModernBoy::operator+(Vec3 lhs, Vec3 rhs){
    return {{lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z}};
}
Vec3& ModernBoy::operator+=(Vec3& lhs, Vec3 rhs){
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    return lhs;
}
Vec3 ModernBoy::operator-(Vec3 lhs, Vec3 rhs){
    return {{lhs.x-rhs.x, lhs.y-rhs.y, lhs.z-rhs.z}};
}
Vec3 ModernBoy::operator-(Vec3 v){
    return {{-v.x, -v.y, -v.z}};
}
Vec3 ModernBoy::operator*(Vec3 v, float f){
    return {{v.x*f, v.y*f, v.z*f}};
}
Vec3& ModernBoy::operator*=(Vec3& v, float f){
    v.x += f;
    v.y += f;
    v.z += f;
    return v;
}
Vec3 ModernBoy::operator/(Vec3 v, float f){
    return {{v.x/f, v.y/f, v.z/f}};
}
Vec3 ModernBoy::cross(Vec3 lhs, Vec3 rhs){
    return {{
        lhs.y*rhs.z - lhs.z*rhs.y,
        lhs.z*rhs.x - lhs.x*rhs.z,
        lhs.x*rhs.y - lhs.y*rhs.x
    }};
}
float ModernBoy::dot(Vec3 lhs, Vec3 rhs){
    return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
}
float ModernBoy::norm_squared(Vec3 v){
    return v.x*v.x + v.y*v.y + v.z*v.z;
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

Camera ModernBoy::defaultCamera(){
    return { CameraType::MainCamera,
        100.0f, 0.1f, 100.0f, Projection::PERSPECTIVE };
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
