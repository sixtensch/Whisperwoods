
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
    sumPos += mul( Tx[input.bones[0]]  * input.weights[0], startPosition );
    sumPos += mul( Tx[input.bones[1]]  * input.weights[1], startPosition );
    sumPos += mul( Tx[input.bones[2]]  * input.weights[2], startPosition );
    sumPos += mul( Tx[input.bones[3]]  * input.weights[3], startPosition );
    //sumPos += mul( Tx[input.bones1[0]] * input.weights1[0], startPosition ); // for extra bone blend at a performance hit
    //sumPos += mul( Tx[input.bones1[1]] * input.weights1[1], startPosition ); // must be matched in pipeline and vertex struct
    //sumPos += mul( Tx[input.bones1[2]] * input.weights1[2], startPosition ); // as well as in the fbx importer, 
    //sumPos += mul( Tx[input.bones1[3]] * input.weights1[3], startPosition ); // requiring a .wmm re-serialization

    output.wPosition = mul(float4(sumPos, 1.0f), WorldMatrix);
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