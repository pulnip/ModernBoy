#include <utility>
#include "Shapes.hpp"

using namespace std;
using namespace DirectX::SimpleMath;
using namespace ModernBoy;

constexpr auto inf = numeric_limits<float>::infinity();
const Hit noHit{-inf, Vector3::Zero, Vector3::Zero};
constexpr auto epsilon = 1e-2f;

ModernBoy::Hit Sphere::shootRay(const Ray& ray) const{
    const auto diff = ray.p0 - center;

    //const auto a = ray.dir.Dot(ray.dir);
    const auto b = ray.dir.Dot(diff);
    const auto c = diff.Dot(diff) - radius * radius;

    const auto disc = b * b - c;
    if (disc < 0.0f) {
        return noHit;
    }

    const auto distance = -b - sqrt(disc);
    const auto point = ray.p0 + distance * ray.dir;
    auto normal = point - center;
    normal.Normalize();

    return {distance, point, normal};
}

ModernBoy::Hit Triangle::shootRay(const Ray& ray) const{
    auto normal=(v1-v0).Cross(v2-v0);
    normal.Normalize();
    const auto cosToFace = -ray.dir.Dot(normal);

    // backface culling.
    if(cosToFace < 0.0f) return noHit;

    // ray-face horizontal check.
    if(abs(cosToFace) < ::epsilon) return noHit;

    const auto diff=ray.p0-v0;
    const auto t=diff.Dot(normal)/cosToFace;

    if(t < 0.0f) return noHit;

    const auto point=ray.p0 + t*ray.dir;

    const auto toV0 = v0-point;
    const auto toV1 = v1-point;
    const auto toV2 = v2-point;

    const auto cross0 = toV1.Cross(toV2);
    const auto cross1 = toV2.Cross(toV0);
    const auto cross2 = toV0.Cross(toV1);

    if(cross0.Dot(normal) < 0.0f) return noHit;
    if(cross1.Dot(normal) < 0.0f) return noHit;
    if(cross2.Dot(normal) < 0.0f) return noHit;

    const auto area0 = cross0.Length() / 2.0f;
    const auto area1 = cross1.Length() / 2.0f;
    const auto area2 = cross2.Length() / 2.0f;

    const auto areaSum = area0 + area1 + area2;

    const auto w0 = area0 / areaSum;
    const auto w1 = area1 / areaSum;
    // const auto w2 = area2 / areaSum;
    const auto w2 = 1.0f - w0 - w1;

    return {t, point, normal,
        w0*uv0 + w1*uv1 + w2*uv2
    };
}

ModernBoy::Hit Square::shootRay(const Ray& ray) const{
    const auto t1 = getTopRight();
    const auto t2 = getBottomLeft();

    const auto hit1 = t1.shootRay(ray);
    const auto hit2 = t2.shootRay(ray);

    const auto hit1Valid = hit1.distance >= 0;
    const auto hit2Valid = hit2.distance >= 0;
    const auto hitValid = (static_cast<int>(hit2Valid)<<1) + static_cast<int>(hit1Valid);
    switch(hitValid){
    case 0b00:
        return noHit;
    case 0b01:
        return hit1;
    case 0b10:
        return hit2;
    case 0b11:
        return (hit1.distance < hit2.distance) ? hit1 : hit2;
    default:
        unreachable();
    }
}

Triangle Square::getTopRight() const{
    Triangle t(v0, v1, v2, uv0, uv1, uv2);

    t.ambient = ambient;
    t.diffuse = diffuse;
    t.specular = specular;

    t.alpha = alpha;
    t.reflection = reflection;
    t.transparency = transparency;

    t.ambTexture = ambTexture;
    t.difTexture = difTexture;

    return t;
}

Triangle Square::getBottomLeft() const{
    Triangle t(v0, v2, v3, uv0, uv2, uv3);

    t.ambient = ambient;
    t.diffuse = diffuse;
    t.specular = specular;

    t.alpha = alpha;
    t.reflection = reflection;
    t.transparency = transparency;

    t.ambTexture = ambTexture;
    t.difTexture = difTexture;

    return t;
}
