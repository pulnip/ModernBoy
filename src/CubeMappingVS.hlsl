#include "CubeMappingCommon.hlsli"

cbuffer VSConstants: register(b0){
    matrix view;
    matrix projection;
};

CubeMapPSInput main(CubeMapVSInput input){
    CubeMapPSInput output;

    float4 pos=float4(input.pos, 1.0);
    output.pos=pos.xyz;

    matrix viewProj=mul(view, projection);
    output.projected=mul(pos, viewProj);

    return output;
}
