#include "Constants.hlsli"

struct VSOutput
{
    float4 outPosition : SV_POSITION;
    float4 wPosition : WPOSITION0;
    float3 outNormal : NORMAL0;
    float3 outTangent : TANGENT0;
    float3 outBitangent : BITANGENT0;
    float2 outUV : TEXCOORD0;
};

cbuffer MaterialInfo : REGISTER_CBV_MATERIAL_INFO
{
    float3 diffuse;
    float alpha;
    float3 specular;
    float glossiness;
    float3 emissive;
    float height;
    float3 pad;
    float tiling;
};

//float2 texOffset(int u, int v, int lNo)
//{
//    return float2(u * 1.0f / 2048, v * 1.0f / 2048);
//}

SamplerState textureSampler : REGISTER_SAMPLER_STANDARD;
//SamplerComparisonState shadowSampler : REGISTER_SAMPLER_SHADOW;

Texture2D textureDiffuse : REGISTER_SRV_TEX_DIFFUSE;
//Texture2D textureSpecular : REGISTER_SRV_TEX_SPECULAR;
//Texture2D textureEmissive : REGISTER_SRV_TEX_EMISSIVE;
//Texture2D textureNormal : REGISTER_SRV_TEX_NORMAL;
//Texture2D shadowTexture : REGISTER_SRV_SHADOW_DEPTH;

//struct PS_OUTPUT
//{
//    //float4 MainTarget : SV_TARGET0;
//    //float4 PositionTarget : SV_TARGET1;
//};

void main(VSOutput input)
{
	// Output struct for both main texture target and positional target.
    //PS_OUTPUT output;
    float2 uv = input.outUV * tiling;	
    float4 diffuseSample = textureDiffuse.Sample(textureSampler, uv);	
    if (diffuseSample.a < 0.1f)
        discard;
   // return output;
}