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