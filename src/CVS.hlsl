cbuffer Constants : register(b0){
    matrix model;
    matrix view;
    matrix projection;
};

struct VSInput{
    float3 pos : POSITION;
    float3 color : COLOR0;
};

struct VSOutput{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

VSOutput main(VSInput input) {
    VSOutput output;
    float4 pos = float4(input.pos, 1.0f);
    pos = mul(pos, model);
    pos = mul(pos, view);
    pos = mul(pos, projection);

    output.pos = pos;
    output.color = input.color;

    return output;
}
