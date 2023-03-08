#include "Constants.hlsli"

struct VSOutput
{
	float4 outPosition	: SV_POSITION;
	float4 wPosition    : WPOSITION0;
	float3 outNormal	: NORMAL0;
	float2 outUV		: TEXCOORD0;
};

cbuffer GUIInfo : REGISTER_CBV_GUI_INFO
{
    float3 colorTint;
    float alpha;
    float3 vectorValue;
    float floatValue;
    int4 intValues;
};

cbuffer PlayerInfo : REGISTER_CBV_PLAYER_INFO
{
    matrix playerMatrix;
};

Texture2D firstTexture : REGISTER_SRV_TEX_DIFFUSE;
Texture2D secondTexture : REGISTER_SRV_TEX_SPECULAR;
SamplerState textureSampler : REGISTER_SAMPLER_STANDARD;
SamplerState textureSamplerNonWrap : REGISTER_SAMPLER_NO_WRAP;

float4 main(VSOutput input) : SV_TARGET0
{
    // For rotating and moving the minimap to match the player... Super yank because of the wack rotations.
    float2 rotatedUV = input.outUV;
    if (intValues.x > 0)
    {
        float4 playerPos = float4(playerMatrix._41, 0, playerMatrix._43, 0);
        float4 uvRotated = mul(float4(input.outUV.y + 0.5f, 0, input.outUV.x - 0.5f, 0), playerMatrix);
        rotatedUV = -float2(uvRotated.z - 0.5f + playerPos.x / 40.0f, uvRotated.x + 0.5f - playerPos.z / 40.0f); // Rotated with yanky player offset. 
    }
    float3 texSample = firstTexture.Sample(textureSampler, rotatedUV).xyz;
    
    // Streching the mask for stamina bar stuff.
    float2 stretchedUV = input.outUV; 
    float maskSample = 1;
    if (intValues.y > 0)
    {
        //stretchedUV.x = stretchedUV.x * floatValue + vectorValue.x;
        //stretchedUV.y = stretchedUV.x + vectorValue.y;
        maskSample = secondTexture.Sample(textureSampler, stretchedUV).x;
        if (maskSample < 0.001f)
            discard;
        return float4(texSample * colorTint, alpha * (sqrt(sqrt(input.outUV.x - (1.0f - floatValue))) * 2.0f) * (maskSample * maskSample*1.5f));
    }
    else
    {
        maskSample = secondTexture.Sample(textureSampler, input.outUV).x;
    }
    
    if (maskSample < 0.001f)
        discard;
    return float4(texSample * colorTint * maskSample, alpha * maskSample);
}

/*        !?

      ("`-''-/").___..--''"`-._
       `6_ 6  )   `-.  (     ).`-.__.`)
       (_Y_.)'  ._   )  `._ `. ``-..-'
     _..`--'_..-_/  /--'_.' ,'
    (il),-''  (li),'  ((!.-'   <( Trial and error mappings... )

    float4 uvRotated = mul(float4(input.outUV.x - 0.5f, 0, input.outUV.y + 0.5f, 0), playerMatrix);
    float2 rotatedUV = float2(-uvRotated.x - 0.5f - playerPos.z / 40.0f, -uvRotated.z + 0.5f + playerPos.x / 40.0f);

    float4 uvRotated = mul(float4(input.outUV.x-0.5f, 0, input.outUV.y+0.5f, 0), playerMatrix);
    float2 rotatedUV = -float2(uvRotated.x+0.5f, uvRotated.z-0.5f); 

    float4 uvRotated = mul(float4(input.outUV.y + 0.5f, 0, input.outUV.x - 0.5f, 0), playerMatrix);
    float2 rotatedUV = -float2(uvRotated.x + 0.5f - playerPos.z / 50.0f, uvRotated.z - 0.5f + playerPos.x / 50.0f);  */