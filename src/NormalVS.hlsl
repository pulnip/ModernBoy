#include "Common.hlsli"

cbuffer VSConstants: register(b0){
    matrix model;
    matrix invTranspose;
    matrix view;
    matrix projection;
};

cbuffer NormalConstants: register(b1){
    float scale;
};

PSInput main(VSInput input){
    PSInput output;

    float4 nml=mul(float4(input.normal, 0.0), invTranspose);
    output.normal=normalize(nml.xyz);
    float t=input.uv.x;

    float4 pos=mul(float4(input.pos, 1.0), model);
    pos.xyz+=scale*t*output.normal;
    output.pos=pos.xyz;

    pos=mul(pos, view);
    pos=mul(pos, projection);
    output.projected=pos;

    float3 lerped=t*float3(0.0, 0.0, 0.0) + (1-t)*float3(0.0, 1.0, 1.0);
    output.color=float4(lerped, 1.0);
    output.uv=input.uv;

    return output;
}
