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

cbuffer PlayerInfo : REGISTER_CBV_PLAYER_INFO
{
    matrix playerMatrix;
};

Texture2D firstTexture : REGISTER_SRV_TEX_DIFFUSE;
Texture2D secondTexture : REGISTER_SRV_TEX_SPECULAR;
SamplerState textureSampler : REGISTER_SAMPLER_STANDARD;

float4 main(VSOutput input) : SV_TARGET0
{
    float4 playerPos = float4(playerMatrix._41, 0, playerMatrix._43, 0);
    
   // float4 uvRotated = mul(float4(input.outUV.x - 0.5f, 0, input.outUV.y + 0.5f, 0), playerMatrix);
    float4 uvRotated = mul(float4(input.outUV.y + 0.5f, 0, input.outUV.x - 0.5f, 0), playerMatrix);
    float2 rotatedUV = float2(uvRotated.x + 0.5f - playerPos.z / 50.0f, uvRotated.z - 0.5f + playerPos.x / 50.0f);

    
    float3 texSample = firstTexture.Sample(textureSampler, rotatedUV);
    float maskSample = secondTexture.Sample(textureSampler, input.outUV);
    if (maskSample < 0.01f)
        discard;
    return float4(texSample * colorTint, 1.0f);
}