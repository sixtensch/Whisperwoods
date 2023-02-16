
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


cbuffer PlayerInfo : REGISTER_CBV_TESSELATION_INFO
{
    matrix playerMatrix;
};

cbuffer ObjectInfo : REGISTER_CBV_OBJECT_INFO
{
    matrix WorldMatrix;
};

VSOutput main(VSInput input)
{
    VSOutput output;

    output.wPosition = mul(float4(input.position, 1.0f), WorldMatrix);

    // Experimental push away
    
    // WIP
    //float4 playerForward = mul( float4(0, 0, 1, 0), playerMatrix );
    //float4 playerRight = mul( float4(1, 0, 0, 0), playerMatrix );
    //playerForward = normalize( playerForward );
    //playerRight = normalize( playerRight );


    float4 playerPos = float4(playerMatrix._41, 0.18f, playerMatrix._43, 1.0f);
    float4 groundedPos = float4(output.wPosition.x, 0.0f, output.wPosition.z, 1.0f);
    float distFromPlayer = distance( playerPos, groundedPos );
    distFromPlayer = sqrt(sqrt(distFromPlayer)); // Controls the falloff curvature

    float4 toFromPlayer = groundedPos - playerPos;
    toFromPlayer = normalize( toFromPlayer );
    toFromPlayer = toFromPlayer * clamp((0.90f - distFromPlayer),0.0f, 0.90f); // controlls range

    // WIP
    /*float tFPR = dot( toFromPlayer, playerRight );
    float tFPF = dot( toFromPlayer, playerForward );
    //toFromPlayer = float4(mul(rotation, toFromPlayer),0.0f);*/

    toFromPlayer = float4(  
        clamp( toFromPlayer.x,-1.0f,1.0f),
        (toFromPlayer.y*0.5f)-(0.7f* clamp((0.8f - distFromPlayer),0,0.8f)), 
        clamp( toFromPlayer.z,-1.0f, 1.0f), 0.0f);

    // WIP
    /* float4 relativeVector = playerForward * toFromPlayer.z;
    relativeVector.y = toFromPlayer.y;*/
    //output.wPosition = output.wPosition + relativeVector *input.UV.w;

    output.wPosition = output.wPosition + toFromPlayer*input.UV.w*1.5f; // Multiply for more effect

    // Normal Shader
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