struct FS_Input{
    float4 position [[position]];
    float3 worldPosition;
    float3 normal;
    float2 uv;
};

struct FS_Input_Line{
    float4 position [[position]];
    float4 color;
};
