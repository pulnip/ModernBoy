#include <metal_stdlib>
using namespace metal;

struct VS_Input{
    float4 position [[attribute(0)]];
    float4 color    [[attribute(1)]];
};
struct VS_Output{
    float4 position [[position]];
    float4 color;
};

vertex VS_Output vertex_main(VS_Input input [[stage_in]]){
    VS_Output output;
    output.position = input.position;
    output.color = input.color;
    return output;
}
fragment float4 fragment_main(VS_Output input [[stage_in]]){
    return input.color;
}
