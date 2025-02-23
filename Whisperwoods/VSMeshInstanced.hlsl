
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
    float4 outUV : TEXCOORD0;
};

cbuffer ViewInfo : REGISTER_CBV_VIEW_INFO
{
    matrix ViewMatrix;
    matrix ProjectionMatrix;
};

cbuffer PlayerInfo : REGISTER_CBV_PLAYER_INFO
{
    matrix playerMatrix;
    float4 worldInfo1;
    float4 worldInfo2;
};

VSOutput main(VSInput input)
{
    VSOutput output;	
    output.wPosition = mul(float4(input.position, 1.0f), input.worldMatrix);

    // Experimental push away...V1
    //float heightEffect = clamp( output.wPosition.y * 0.6f, 0.0f, 0.6f );
    float4 playerPos = float4(playerMatrix._41, 0.15f, playerMatrix._43, 1.0f);
    float4 playerForward = normalize(mul(float4(0, 0, 1, 0), playerMatrix));
    float4 playerRight = normalize(mul(float4(1, 0, 0, 0), playerMatrix));
    float4 toFrom2 = output.wPosition - playerPos;
    float dotHor = abs(dot(toFrom2, playerForward));
    float dotVert = abs(dot(toFrom2, playerRight));
    float4 groundedPos = float4(output.wPosition.x, 0.0f, output.wPosition.z, 1.0f);
    float distFromPlayer = sqrt(sqrt(distance(playerPos, groundedPos)));
    float4 toFromPlayer = groundedPos - playerPos;
    float distControl = clamp((0.80f - distFromPlayer), 0.0f, 0.80f); // controlls range
    toFromPlayer = normalize(toFromPlayer);
    toFromPlayer = toFromPlayer * distControl;
    toFromPlayer = float4(
        clamp(toFromPlayer.x, -1.0f, 1.0f) * (dotVert * 4.0f),
        (toFromPlayer.y * 0.2f) - (0.7f * distControl * 1.1f),
        clamp(toFromPlayer.z, -1.0f, 1.0f) * (dotVert * 4.0f), 0.0f);
    output.wPosition = output.wPosition + (toFromPlayer * input.UV.w * 1.4f * min(max(2.0 - output.wPosition.y, 0.0f), 1.0f)); // Multiply for more effect
    output.wPosition = float4(output.wPosition.x, clamp(output.wPosition.y, 0.001f, 50000), output.wPosition.z, output.wPosition.w);

    // Normal Shader cont...
    
    float sinCosX = sin(cos(output.wPosition.x));
    output.outPosition = mul(mul(output.wPosition, ViewMatrix), ProjectionMatrix) 
    + (float4(
    (sinCosX * (worldInfo1.x) * 2) * 0.01f,
    (sinCosX * (worldInfo1.x) * 2) * 0.01f,
    sin(cos(output.wPosition.z) * (worldInfo1.y) * 2) * 0.01f, 0)
    * min(sin(output.wPosition.y), 1.0f) * input.UV.w)
    ;

    output.outNormal = mul(input.normal, (float3x3) input.worldMatrix);
    //output.outNormal = normalize(output.outNormal);

    output.outTangent = mul(input.tangent, (float3x3) input.worldMatrix);
    //output.outTangent = normalize(output.outTangent);

    output.outBitangent = mul(input.bitangent, (float3x3) input.worldMatrix);
    //output.outBitangent = normalize(output.outBitangent);
    
    output.outUV = float4(input.UV.x, -input.UV.y, (sinCosX * worldInfo1.x), 0);
    return output;
}