struct PixelShaderInput{
    float4 pos: SV_POSITION;
    float3 normal: NORMAL;
    float4 color: COLOR;
    float2 uv: TEXCOORD;
};

float4 main(PixelShaderInput input) : SV_TARGET{
    // Use the interpolated vertex color
    return input.color;
}
