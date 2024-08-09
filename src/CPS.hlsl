Texture2D gtexture: register(t0);
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
    if(input.uv.x<xSplit || xSplit==1.0f)
        return float4(1.0f, 1.0f, 1.0f, 1.0f);

    return gtexture.Sample(gsampler, input.uv);
    // Use the interpolated vertex color
    // return input.color;
}
