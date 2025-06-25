#include <limits>
#include "common/type.hpp"

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

Camera ModernBoy::defaultCamera(){
    return { 100.0f, 0.1f, 100.0f, Projection::PERSPECTIVE };
}

Handle ModernBoy::invalidHandle(){
    return {std::numeric_limits<size_t>::max(), 0};
}
bool ModernBoy::isValid(Handle handle){
    return handle.index != std::numeric_limits<size_t>::max();
}
bool ModernBoy::operator==(const Handle& lhs, const Handle& rhs){
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
