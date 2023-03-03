#include "Constants.hlsli"

struct VSOutput
{
	float4 outPosition	: SV_POSITION;
	float4 wPosition    : WPOSITION0;
	float3 outNormal	: NORMAL0;
	float2 outUV		: TEXCOORD0;
};

cbuffer GUIInfo : REGISTER_CBV_GUI_INFO
{
    float3 colorTint;
    float alpha;
    float3 playerPos;
    float stamina;
};

Texture2D firstTexture : REGISTER_SRV_TEX_DIFFUSE;
Texture2D secondTexture : REGISTER_SRV_TEX_SPECULAR;

SamplerState textureSampler : REGISTER_SAMPLER_STANDARD;

float4 main(VSOutput input) : SV_TARGET0
{
    float3 texSample = firstTexture.Sample(textureSampler, input.outUV);
    float maskSample = secondTexture.Sample(textureSampler, input.outUV);
    if (maskSample < 0.01f)
        discard;
    return float4(texSample * colorTint, 1.0f);
}