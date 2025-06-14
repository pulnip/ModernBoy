#include <metal_stdlib>
using namespace metal;

constant float3 lightDirection = float3(0.4, -0.8, 0.2);

constant float3 lightDiffuse = float3(1.0);
constant float3 lightSpecular = float3(0.5);
constant float3 lightAmbient = float3(0.2);
inline float3 phongLighting(float3 diffuseColor, float3 normal,
    float3 worldPosition, float3 viewPosition
){
    float3 diff = max(dot(normal, -lightDirection), 0.0);
    float3 diffuse = lightDiffuse * diff * diffuseColor;

    float3 viewDir = normalize(viewPosition - worldPosition);
    float3 reflectDir = reflect(lightDirection, normal);
    float3 spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    float3 specular = spec * lightSpecular;

    float3 ambient = lightAmbient * diffuseColor;

    return diffuse + ambient;
}

struct SmolVertex{
    float4 position [[attribute(0)]];
    float4 color    [[attribute(1)]];
};
struct Vertex{
    float4 position [[attribute(0)]];
    float4 normal   [[attribute(1)]];
    float4 uv       [[attribute(2)]];
};
struct VS_Output{
    float4 position [[position]];
    float3 worldPosition;
    float3 normal;
    float2 uv;
};

vertex VS_Output vertex_main(uint vid [[vertex_id]],
    constant Vertex* vertices [[buffer(0)]],
    constant float4x4& proj [[buffer(1)]],
    constant float4x4& view [[buffer(2)]],
    constant float4x4& model [[buffer(3)]]
){
    VS_Output output;
    output.worldPosition = (model*vertices[vid].position).xyz;
    output.position = proj * view * float4(output.worldPosition, 1.0);
    output.normal = vertices[vid].normal.xyz;
    output.uv = vertices[vid].uv.xy;
    return output;
}
fragment float4 fragment_main(
    VS_Output input [[stage_in]],
    constant float3& viewPosition [[buffer(0)]],
    texture2d<float> tex [[texture(0)]],
    sampler samp [[sampler(0)]]
){
    float4 color = float4(input.uv, 0.0, 0.5);
    // float2 uv = input.uv.xy;
    // float4 color = tex.sample(samp, uv);
    return float4(phongLighting(color.rgb, input.normal,
        input.worldPosition, viewPosition
    ), 1.0);
    return color;
}
