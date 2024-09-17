#pragma once

struct CubeMapVSInput{
    float3 pos: POSITION;
};

struct CubeMapPSInput{
    float4 projected: SV_POSITION;
    float3 pos: POSITION;
};
