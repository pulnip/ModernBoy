#import <metal_stdlib>

using namespace metal;

struct PhongLight{
    float3 direction;
    float3 diffuse;
    float3 specular;
    float3 ambient;
};
inline float3 phongLighting(
    float3 baseColor, float3 normal,
    float3 worldPosition, float3 viewPosition,
    PhongLight light
){
    float3 diffuse = max(dot(normal, -light.direction), 0.0);
    float3 diffuseColor = light.diffuse * diffuse * baseColor;

    float3 viewDir = normalize(viewPosition - worldPosition);
    float3 reflectDir = reflect(light.direction, normal);
    float3 specular = pow(max(dot(viewDir, reflectDir), 0.0), 1);
    float3 specularColor = specular * light.specular;

    float3 ambientColor = light.ambient * diffuseColor;

    return diffuseColor + specularColor + ambientColor;
}

struct RimConstant{
    float3 rimColor;
    float rimPower;
    float rimStrength;
};

constant float3 rimBase = float3(0.2);
constant float rimPower = 10.0;
constant float rimStrength = 0.5;

inline float3 rimLighting(
    float3 baseColor, float3 normal,
    float3 worldPosition, float3 viewPosition
    // constant RimConstant& constants
){
    float3 toView = normalize(viewPosition - worldPosition);
    float rimFactor = smoothstep(0.0, 1.0, 1.0 - dot(toView, normal));
    float rim = pow(rimFactor, rimPower);
    float3 rimColor = rimStrength * rim * rimBase;
    return baseColor + rimColor;
}
