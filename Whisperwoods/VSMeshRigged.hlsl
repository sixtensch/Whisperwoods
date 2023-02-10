
#include "Constants.hlsli"

struct VSInput
{
    float3 position     : POSITION0;
    float3 normal       : NORMAL0;
    float3 tangent      : TANGENT0;
    float3 bitangent    : BITANGENT0;
    float4 UV           : TEXCOORD0;
    int4   bones        : BONES0;
    float4 weights      : WEIGHTS0;
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

cbuffer VSViewInfo : REGISTER_CBV_VIEW_INFO
{
    matrix ViewMatrix;
    matrix ProjectionMatrix;
};

cbuffer VSObjectInfo : REGISTER_CBV_OBJECT_INFO
{
    matrix WorldMatrix;
};

StructuredBuffer<float4x4> Tx : REGISTER_SRV_ARMATURE_MATRIX;

VSOutput main(VSInput input)
{
    VSOutput output;
	
    float4 startPosition = float4(input.position, 1.0f);
    float3 sumPos = float3(0, 0, 0);
    sumPos += mul(Tx[input.bones[0]] * input.weights[0], startPosition).xyz;
    sumPos += mul(Tx[input.bones[1]] * input.weights[1], startPosition).xyz;
    sumPos += mul(Tx[input.bones[2]] * input.weights[2], startPosition).xyz;
    sumPos += mul(Tx[input.bones[3]] * input.weights[3], startPosition).xyz;
    //output.outPosition = float4(sumPos, 1.0f);
    //output.outPosition = mul(output.position, worldMatrix);
    //output.outPosition = mul(output.position, ViewMatrix);
    //output.outPosition = mul(output.position, ProjectionMatrix);

    output.wPosition = mul(float4(input.position, 1.0f), WorldMatrix);
    output.wsPosition = mul(output.wPosition, ViewMatrix);
    output.outPosition = mul(output.wsPosition, ProjectionMatrix);

    output.outNormal = mul(input.normal, (float3x3)WorldMatrix);
    //output.outNormal = normalize(output.outNormal);

    output.outTangent = mul(input.tangent, (float3x3)WorldMatrix);
    //output.outTangent = normalize(output.outTangent);

    output.outBitangent = mul(input.bitangent, (float3x3)WorldMatrix);
    //output.outBitangent = normalize(output.outBitangent);

    output.outUV = input.UV;
    return output;
}