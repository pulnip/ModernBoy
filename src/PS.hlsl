#include "Common.hlsli"

Texture2D gtexture0: register(t0);
Texture2D gtexture1: register(t1);
SamplerState gsampler: register(s0);

cbuffer PSConstants: register(b0){
    // float xSplit;
    // float3 dummy;
    float3 eyePos;
    bool useTexture;
    Material material;
    Light lights[MAX_LIGHTS];
};

float4 main(PSInput input): SV_TARGET{
    float3 toEye=normalize(eyePos-input.pos);

    float3 color=float3(0.0, 0.0, 0.0);

    int i=0;

    [unroll]
    for(i=0; i<NUM_DIR_LIGHT; ++i){
        color += dirLight(lights[i], material, input.pos, input.normal, toEye);
    }
    [unroll]
    for(i=NUM_DIR_LIGHT; i<NUM_DIR_LIGHT+NUM_POINT_LIGHT; ++i){
        color += pointLight(lights[i], material, input.pos, input.normal, toEye);
    }
    [unroll]
    for(i=NUM_DIR_LIGHT+NUM_POINT_LIGHT; i<NUM_DIR_LIGHT+NUM_POINT_LIGHT+NUM_SPOT_LIGHT; ++i){
        color += spotLight(lights[i], material, input.pos, input.normal, toEye);
    }

    return useTexture ?
        float4(color, 1.0)*gtexture0.Sample(gsampler, input.uv):
        float4(color, 1.0);
}
