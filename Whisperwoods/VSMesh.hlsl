
#include "Constants.hlsli"

struct VSInput
{
    float3 position     : POSITION0;
    float3 normal       : NORMAL0;
    float3 tangent      : TANGENT0;
    float3 bitangent    : BITANGENT0;
    float4 UV           : TEXCOORD0;
}; 

struct VSOutput
{
    float4 outPosition  : SV_POSITION;
    float4 wPosition    : WPOSITION0;
    float4 wsPosition   : WSPOSITION0;
    float3 outNormal    : NORMAL0;
    float3 outTangent   : TANGENT0;
    float3 outBitangent : BITANGENT0;
    float2 outUV        : TEXCOORD0;
};

cbuffer ViewInfo : REGISTER_CBV_VIEW_INFO
{
    matrix ViewMatrix;
    matrix ProjectionMatrix;
};

cbuffer ObjectInfo : REGISTER_CBV_OBJECT_INFO
{
    matrix WorldMatrix;
};

VSOutput main(VSInput input)
{
    VSOutput output;
	
    output.wPosition = mul(float4(input.position, 1.0f), WorldMatrix);
    output.wsPosition = mul(output.wPosition, ViewMatrix);
    output.outPosition = mul(output.wsPosition, ProjectionMatrix);

    output.outNormal = mul(input.normal, (float3x3)WorldMatrix);
    //output.outNormal = normalize(output.outNormal);

    output.outTangent = mul(input.tangent, (float3x3)WorldMatrix);
    //output.outTangent = normalize(output.outTangent);

    output.outBitangent = mul(input.bitangent, (float3x3)WorldMatrix);
    //output.outBitangent = normalize(output.outBitangent);
    
    output.outUV = float2(input.UV.x, -input.UV.y);
	return output;
}