#include <metal_stdlib>
using namespace metal;

struct VS_Input{
    float3 position [[attribute(0)]];
    float3 color    [[attribute(1)]];
};
struct VS_Output{
    float4 position [[position]];
    float3 color;
};

vertex VS_Output vertex_main(VS_Input input [[stage_in]]){
    VS_Output output;
    output.position = float4(input.position, 1.0);
    output.color = input.color;
    return output;
}
fragment float4 fragment_main(VS_Output input [[stage_in]]){
    return float4(input.color, 1.0);
}
