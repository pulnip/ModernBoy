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

vertex VS_Output vertex_main(Vertex input [[stage_in]]){
    VS_Output output;
    output.position = input.position;
    output.uv = input.uv;
    return output;
}
fragment float4 fragment_main(
    VS_Output input  [[stage_in]],
    texture2d<float> tex [[texture(0)]],
    sampler samp     [[sampler(0)]]
){
    float2 uv = input.uv.xy;
    float4 color = tex.sample(samp, uv);
    return color;
}
