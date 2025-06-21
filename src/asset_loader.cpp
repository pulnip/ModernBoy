#include "asset_loader.hpp"

using namespace ModernBoy;

Transform ModernBoy::parseTransform(
    const toml::v3::table& table
){
    Transform transform{};
    auto p = table["position"].as_array();
    auto r = table["rotation"].as_array();
    auto s = table["scale"].as_array();

    for(size_t i=0; i<3; ++i)
        transform.pos[i] = *((*p)[i]).value<double>();
    for(size_t i=0; i<4; ++i)
        transform.rot[i] = *((*r)[i]).value<double>();
    for(size_t i=0; i<3; ++i)
        transform.scl[i] = *((*s)[i]).value<double>();
    return transform;
}

Camera ModernBoy::parseCamera(const toml::v3::table& table){
    auto fov = *table["fov"].value<double>();
    auto near = *table["nearPlane"].value<double>();
    auto far = *table["farPlane"].value<double>();
    auto proj = *table["projection"].value<std::string>();

    Projection projection = Projection::PERSPECTIVE;
    if(proj.compare("orthographic") == 0)
        projection = Projection::ORTHOGRAPHIC;

    return Camera{
        .fov = static_cast<float>(fov),
        .nearPlane = static_cast<float>(near),
        .farPlane = static_cast<float>(far),
        .projection = projection
    };
}

