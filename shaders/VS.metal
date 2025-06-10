#include <metal_stdlib>
using namespace metal;

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
    float4 uv;
};

vertex VS_Output vertex_main(uint vid [[vertex_id]],
    constant Vertex* vertices [[buffer(0)]],
    constant float4x4& proj [[buffer(1)]],
    constant float4x4& view [[buffer(2)]],
    constant float4x4& model [[buffer(3)]]
){
    VS_Output output;
    output.position = (model*vertices[vid].position);
    output.position = proj * view * output.position;
    output.uv = vertices[vid].uv;
    return output;
}
fragment float4 fragment_main(
    VS_Output input [[stage_in]],
    texture2d<float> tex [[texture(0)]],
    sampler samp [[sampler(0)]]
){
    float2 uv = input.uv.xy;
    float4 color = tex.sample(samp, uv);
    return color;
}
