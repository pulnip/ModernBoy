#include "Common.hlsli"

cbuffer VSConstants: register(b0){
    matrix model;
    matrix invTranspose;
    matrix view;
    matrix projection;
};

PSInput main(VSInput input){
    PSInput output;

    float4 pos=mul(float4(input.pos, 1.0f), model);
    output.pos=pos.xyz;

    pos=mul(pos, view);
    pos=mul(pos, projection);
    output.projected=pos;

    output.normal=normalize(
        mul(float4(input.normal, 0.0), invTranspose).xyz
    );
    // output.color=input.color;
    output.uv=input.uv;

    return output;
}
