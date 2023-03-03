#include "Constants.hlsli"

struct VSInput
{
    float3 position     : POSITION0;
    float3 normal       : NORMAL0;
    float4 UV           : TEXCOORD0;
};

struct VSOutput
{
    float4 outPosition  : SV_POSITION;
    float4 wPosition    : WPOSITION0;
    float3 outNormal    : NORMAL0;
    float2 outUV        : TEXCOORD0;
};

//cbuffer ViewInfo : REGISTER_CBV_VIEW_INFO
//{
//    matrix ViewMatrix;
//    matrix ProjectionMatrix;
//};

cbuffer VSObjectInfo : REGISTER_CBV_OBJECT_INFO
{
    matrix WorldMatrix;
};

VSOutput main( VSInput input )
{
    VSOutput output;

    output.wPosition = mul(float4(input.position, 1.0f), WorldMatrix);
    //output.wPosition = float4(input.position, 1.0f);

    //output.outPosition = mul(float4(input.position, 1.0f), WorldMatrix);
    output.outPosition = output.wPosition;

    output.outNormal = mul(input.normal, (float3x3)WorldMatrix);

    output.outUV = float2(input.UV.x, -input.UV.y);
	return output;
}