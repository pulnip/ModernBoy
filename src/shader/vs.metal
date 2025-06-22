#import <metal_stdlib>
#import "common.metal"

using namespace metal;

struct Vertex{
    float4 position             [[attribute(0)]];
    float4 normal               [[attribute(1)]];
    float4 uv                   [[attribute(2)]];
};
struct ViewConstant{
    float4x4 trs;
    float4x4 proj;
};
struct ModelConstant{
    float4x4 trs;
    float3x3 normal;
};

vertex FS_Input vertex_main(uint vid [[vertex_id]],
    constant Vertex* vertices     [[buffer(0)]],
    constant ViewConstant& view   [[buffer(1)]],
    constant ModelConstant& model [[buffer(2)]]
){
    FS_Input output;
    output.worldPosition = (model.trs*vertices[vid].position).xyz;
    output.position = view.proj * view.trs * float4(output.worldPosition, 1.0);
    output.normal = model.normal * vertices[vid].normal.xyz;
    output.uv = vertices[vid].uv.xy;
    return output;
}
