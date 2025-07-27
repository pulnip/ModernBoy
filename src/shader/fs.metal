#import "common.metal"
#import "lighting.metal"

constant float3 lightDirection = float3(0.436436, -0.872872, 0.218218);

constant float3 lightDiffuse = float3(1.0);
constant float3 lightSpecular = float3(0.5);
constant float3 lightAmbient = float3(0.2);

fragment float4 fragment_main(
    FS_Input input                [[stage_in]],
    constant float3& viewPosition [[buffer(0)]],
    texture2d<float> tex          [[texture(0)]],
    sampler samp                  [[sampler(0)]],
    constant RimConstant& rimc    [[buffer(1)]],
    constant float& alpha         [[buffer(2)]]
){
    float2 uv = input.uv.xy;
    float4 color = tex.sample(samp, uv);

    PhongLight light{
        lightDirection,
        lightDiffuse,
        lightSpecular,
        lightAmbient
    };

    float3 phongColor = phongLighting(color.rgb, input.normal,
        input.worldPosition, viewPosition, light);
    float3 rimColor = rimLighting(color.rgb, input.normal,
        input.worldPosition, viewPosition, rimc);
    float3 lightingColor = phongColor + rimColor;

    float3 red = float3(1.0, 0.0, 0.0);

    return float4(mix(red, lightingColor, alpha), color.a);
}
