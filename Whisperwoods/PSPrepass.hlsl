#include "Constants.hlsli"

Texture2D textureDiffuse : REGISTER_SRV_TEX_DIFFUSE;
SamplerState textureSampler : REGISTER_SAMPLER_STANDARD;

struct VSOutput
{
    float4 outPosition  : SV_POSITION;
    float4 wPosition    : WPOSITION0;
    float3 outNormal    : NORMAL0;
    float3 outTangent   : TANGENT0;
    float3 outBitangent : BITANGENT0;
    float2 outUV        : TEXCOORD0;
};

void main(VSOutput input)
{
    if (textureDiffuse.Sample(textureSampler, input.outUV).a < 0.1f)
        discard;
    return;
}