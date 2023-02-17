
#include "Constants.hlsli"

struct VSInput
{
    float3 position : POSITION0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float3 bitangent : BITANGENT0;
    float4 UV : TEXCOORD0;
    
    matrix worldMatrix : WORLDMATRIX;
};

struct VSOutput
{
    float4 outPosition : SV_POSITION;
    float4 wPosition : WPOSITION0;
    float3 outNormal : NORMAL0;
    float3 outTangent : TANGENT0;
    float3 outBitangent : BITANGENT0;
    float2 outUV : TEXCOORD0;
};

cbuffer ViewInfo : REGISTER_CBV_VIEW_INFO
{
    matrix ViewMatrix;
    matrix ProjectionMatrix;
};

VSOutput main(VSInput input)
{
    VSOutput output;
	
    output.wPosition = mul(float4(input.position, 1.0f), input.worldMatrix);
    output.outPosition = mul(mul(output.wPosition, ViewMatrix), ProjectionMatrix);

    output.outNormal = mul(input.normal, (float3x3) input.worldMatrix);
    //output.outNormal = normalize(output.outNormal);

    output.outTangent = mul(input.tangent, (float3x3) input.worldMatrix);
    //output.outTangent = normalize(output.outTangent);

    output.outBitangent = mul(input.bitangent, (float3x3) input.worldMatrix);
    //output.outBitangent = normalize(output.outBitangent);
    
    output.outUV = float2(input.UV.x, -input.UV.y);
    return output;
}