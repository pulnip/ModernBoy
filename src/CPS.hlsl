cbuffer PSConstants{
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
    if(input.uv.x < xSplit)
        input.color=float4(1.0f, 1.0f, 1.0f, 1.0f);

    // Use the interpolated vertex color
    return input.color;
}
