#pragma once

#define MAX_LIGHTS 3
#define NUM_DIR_LIGHT 1
#define NUM_POINT_LIGHT 1
#define NUM_SPOT_LIGHT 1

struct Material{
    float3 ambient;
    float shininess;
    float3 diffuse;
    float dummy1;
    float3 specular;
    float dummy2;
};

struct Light{
    float3 strength;
    float fallOffStart;
    float3 direction;
    float fallOffEnd;
    float3 position;
    float spotPower;
};

float attenuate(float val, float start, float end){
    return saturate((end-val) / (end-start));
}

float3 blinnPhong(float3 str, float3 dir, float3 normal, float3 toEye, Material m){
    float3 halfway=normalize(toEye+dir);
    float cosH=dot(normal, halfway);

    float3 specular=m.specular*pow(max(0.0f, cosH), 2.0*m.shininess);

    return m.ambient + (m.diffuse + specular)*str;
}

float3 phong(float3 str, float3 dir, float3 normal, float3 toEye, Material m){
    float3 refl=-reflect(dir, normal);
    float cosR=dot(toEye, refl);

    float3 specular=m.specular*pow(max(cosR, 0.0), m.shininess);

    return m.ambient + (m.diffuse + specular)*str;
}

float3 dirLight(Light l, Material m, float3 pos, float3 normal, float3 toEye){
    float3 toLight=-l.direction;
    float cosToLight=max(0.0, dot(normal, toLight));

    float str=cosToLight*l.strength;

    return blinnPhong(str, toLight, normal, toEye, m);
}

float3 pointLight(Light l, Material m, float3 pos, float3 normal, float3 toEye){
    float3 toLight=l.position-pos;
    float dist=length(toLight);

    if(l.fallOffEnd < dist){
        return float3(0.0, 0.0, 0.0);
    }
    toLight/=dist;

    float cosToLight=max(0.0, dot(normal, toLight));
    float att=attenuate(dist, l.fallOffStart, l.fallOffEnd);

    float3 str=att*cosToLight*l.strength;

    return blinnPhong(str, toLight, normal, toEye, m);
}

float spotLight(Light l, Material m, float3 pos, float3 normal, float3 toEye){
    float3 toLight=l.position-pos;
    float dist=length(toLight);

    if(l.fallOffEnd < dist){
        return float3(0.0, 0.0, 0.0);
    }
    toLight/=dist;

    float cosToLight=max(0.0, dot(normal, toLight));
    float att=attenuate(dist, l.fallOffStart, l.fallOffEnd);
    float cosToEye=max(0.0, dot(l.direction, -toLight));
    float power=pow(cosToEye, l.spotPower);

    float3 str=power*att*cosToLight*l.strength;

    return blinnPhong(str, toLight, normal, toEye, m);
}

struct VSInput{
    float3 pos: POSITION;
    float3 normal: NORMAL;
    // float4 color: COLOR0;
    float2 uv: TEXCOORD;
};

struct PSInput{
    float4 projected: SV_POSITION;
    float3 pos: POSITION;
    float3 normal: NORMAL;
    float4 color: COLOR;
    float2 uv: TEXCOORD;
};
