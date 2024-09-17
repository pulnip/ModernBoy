#include "CubeMappingCommon.hlsli"

TextureCube gtexture: register(t0);
SamplerState gsampler: register(s0);

float4 main(CubeMapPSInput input): SV_TARGET{
    return gtexture.Sample(gsampler, input.pos.xyz);
}
