#include "Common.hlsli"

cbuffer VSConstants: register(b0){
    matrix model;
    matrix invTranspose;
    matrix view;
    matrix projection;
};

PSInput main(VSInput input){
    PSInput output;

    float4 pos=mul(float4(input.pos, 1.0), model);
    output.pos=pos.xyz;

    pos=mul(pos, view);
    pos=mul(pos, projection);
    output.projected=pos;

    float4 nml=mul(float4(input.normal, 0.0), invTranspose);
    output.normal=normalize(nml.xyz);
    output.color=float4(0.0, 0.0, 0.0, 1.0);
    output.uv=input.uv;

    return output;
}
