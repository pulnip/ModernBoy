#import <metal_stdlib>
#import "common.metal"

using namespace metal;

struct Vertex{
    float4 position [[attribute(0)]];
    float4 normal   [[attribute(1)]];
    float4 uv       [[attribute(2)]];
};

vertex FS_Input vertex_main(uint vid [[vertex_id]],
    constant Vertex* vertices [[buffer(0)]],
    constant float4x4& proj   [[buffer(1)]],
    constant float4x4& view   [[buffer(2)]],
    constant float4x4& model  [[buffer(3)]],
    constant float3x3& normal [[buffer(4)]]
){
    FS_Input output;
    output.worldPosition = (model*vertices[vid].position).xyz;
    output.position = proj * view * float4(output.worldPosition, 1.0);
    output.normal = normal * vertices[vid].normal.xyz;
    output.uv = vertices[vid].uv.xy;
    return output;
}
