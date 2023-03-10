
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
    //int4   bones1       : BONESTWO0;
    //float4 weights1     : WEIGHTSTWO0;
};

struct VSOutput
{
    float4 outPosition  : SV_POSITION;
    float4 wPosition    : WPOSITION0;
    float3 outNormal    : NORMAL0;
    float3 outTangent   : TANGENT0;
    float3 outBitangent : BITANGENT0;
    float4 outUV        : TEXCOORD0;
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
    sumPos += mul( Tx[input.bones[0]]  * input.weights[0], startPosition ).xyz;
    sumPos += mul( Tx[input.bones[1]]  * input.weights[1], startPosition ).xyz;
    sumPos += mul( Tx[input.bones[2]]  * input.weights[2], startPosition ).xyz;
    sumPos += mul( Tx[input.bones[3]]  * input.weights[3], startPosition ).xyz;
    //sumPos += mul( Tx[input.bones1[0]] * input.weights1[0], startPosition ); // for extra bone blend at a performance hit
    //sumPos += mul( Tx[input.bones1[1]] * input.weights1[1], startPosition ); // must be matched in pipeline and vertex struct
    //sumPos += mul( Tx[input.bones1[2]] * input.weights1[2], startPosition ); // as well as in the fbx importer, 
    //sumPos += mul( Tx[input.bones1[3]] * input.weights1[3], startPosition ); // requiring a .wmm re-serialization

    output.wPosition = mul(float4(sumPos, 1.0f), WorldMatrix);
    output.outPosition = mul(mul(output.wPosition, ViewMatrix), ProjectionMatrix);


    float4 startNormal = float4(input.normal, 0.0f);
    float3 sumNormal = float3(0, 0, 0);
    sumNormal += mul(Tx[input.bones[0]] * input.weights[0], startNormal).xyz;
    sumNormal += mul(Tx[input.bones[1]] * input.weights[1], startNormal).xyz;
    sumNormal += mul(Tx[input.bones[2]] * input.weights[2], startNormal).xyz;
    sumNormal += mul(Tx[input.bones[3]] * input.weights[3], startNormal).xyz;
    output.outNormal = mul(sumNormal, (float3x3)WorldMatrix);
    output.outNormal = normalize(output.outNormal);

    float4 startTangent = float4(input.tangent, 1.0f);
    float3 sumTangent = float3(0, 0, 0);
    sumTangent += mul(Tx[input.bones[0]] * input.weights[0], startTangent).xyz;
    sumTangent += mul(Tx[input.bones[1]] * input.weights[1], startTangent).xyz;
    sumTangent += mul(Tx[input.bones[2]] * input.weights[2], startTangent).xyz;
    sumTangent += mul(Tx[input.bones[3]] * input.weights[3], startTangent).xyz;
    output.outTangent = mul(sumTangent, (float3x3)WorldMatrix);
    output.outTangent = normalize(output.outTangent);

    float4 startBiTangent = float4(input.bitangent, 1.0f);
    float3 sumBiTangent = float3(0, 0, 0);
    sumBiTangent += mul(Tx[input.bones[0]] * input.weights[0], startBiTangent).xyz;
    sumBiTangent += mul(Tx[input.bones[1]] * input.weights[1], startBiTangent).xyz;
    sumBiTangent += mul(Tx[input.bones[2]] * input.weights[2], startBiTangent).xyz;
    sumBiTangent += mul(Tx[input.bones[3]] * input.weights[3], startBiTangent).xyz;
    output.outBitangent = mul(sumBiTangent, (float3x3)WorldMatrix);
    output.outBitangent = normalize(output.outBitangent);

    // Old setup (unweighted normals)
    //output.wPosition = mul(float4(sumPos, 1.0f), WorldMatrix);
    //output.wsPosition = mul(output.wPosition, ViewMatrix);
    //output.outPosition = mul(output.wsPosition, ProjectionMatrix);

    //output.outNormal = mul(input.normal, (float3x3)WorldMatrix);
    ////output.outNormal = normalize(output.outNormal);

    //output.outTangent = mul(input.tangent, (float3x3)WorldMatrix);
    ////output.outTangent = normalize(output.outTangent);

    //output.outBitangent = mul(input.bitangent, (float3x3)WorldMatrix);
    ////output.outBitangent = normalize(output.outBitangent);

    output.outUV = float4(input.UV.x, -input.UV.y,0,0);
    return output;
}