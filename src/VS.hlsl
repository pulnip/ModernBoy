cbuffer VSConstants: register(b0){
    matrix model;
    matrix view;
    matrix projection;
};

struct VSInput{
    float3 pos: POSITION;
    float3 normal: NORMAL;
    float4 color: COLOR0;
    float2 uv: TEXCOORD;
};

struct VSOutput{
    float4 pos: SV_POSITION;
    float3 normal: NORMAL;
    float4 color: COLOR;
    float2 uv: TEXCOORD;
};

VSOutput main(VSInput input){
    VSOutput output;

    float4 pos=float4(input.pos, 1.0f);
    pos=mul(pos, model);
    pos=mul(pos, view);
    pos=mul(pos, projection);

    output.pos=pos;
    output.normal=input.normal;
    output.color=input.color;
    output.uv=input.uv;

    return output;
}
