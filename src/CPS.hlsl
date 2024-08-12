Texture2D gtexture0: register(t0);
Texture2D gtexture1: register(t1);
SamplerState gsampler: register(s0);

cbuffer PSConstants: register(b0){
    float xSplit;
    float3 dummy;
};

struct PixelShaderInput{
    float4 pos: SV_POSITION;
    float3 normal: NORMAL;
    float4 color: COLOR;
    float2 uv: TEXCOORD;
};

float4 main(PixelShaderInput input): SV_TARGET{
    float4 color0=gtexture0.Sample(gsampler, input.uv);
    float4 color1=gtexture1.Sample(gsampler, input.uv);

    // if(distance(input.uv, float2(0.5, 0.5)) < 0.5*xSplit)
    //     return 10*color;
    if(input.uv.x<xSplit || xSplit==1.0f)
        return color0;

    return color1;
    // Use the interpolated vertex color
    // return input.color;
}
