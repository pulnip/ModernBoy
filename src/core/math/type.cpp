#include <limits>
#include <unordered_map>
#include "core/math/type.hpp"
#include "core/string/helper.hpp"

using namespace ModernBoy;

static std::unordered_map<std::string, CameraType>
text2camera = {
    {"MAINCAMERA", CameraType::MainCamera},
    { "SUBCAMERA",  CameraType::SubCamera},
};
auto ModernBoy::cameraType(const std::string& text)->CameraType{
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
auto ModernBoy::projection(const std::string& text)->Projection{
    auto upper = toUpper(text);
        auto it = text2projection.find(upper);
    if (it == text2projection.end()){
        return Projection::UNKNOWN;
    }
    return it->second;
}
