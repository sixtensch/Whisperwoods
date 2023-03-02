#include "Constants.hlsli"
#include "ComputeConstants.hlsli"




struct LightDirectional
{
    matrix clip;
    float3 direction;
    float pad1;
    float3 intensity;
    float pad2;
};

struct LightPoint
{
    float3 position;
    float range;
    float3 intensity;
    float pad1;
};

struct LightSpot
{
    matrix clip;
    float3 direction;
    float range;
    float3 position;
    float cosInner;
    float3 intensity;
    float cosOuter;
};

cbuffer ShadingInfo : REGISTER_CBV_SHADING_INFO
{
    LightDirectional directionalLight;
    LightPoint pointLights[LIGHT_CAPACITY_POINT];
    LightSpot spotLights[LIGHT_CAPACITY_SPOT];
	
    float3 ambient;
    uint pointCount;
    float3 cameraPosition;
    uint spotCount;
};

cbuffer TIME_SWITCH_INFO_BUFFER : REGISTER_CBV_SWITCH_INFO
{
    float timeSinceSwitch;
    float timeSwitchStartDuration;
    float timeSwitchEndDuration;
    float isInFuture;
    
    float detectionLevel;
    float PADDING[3];
}


cbuffer ENEMY_CONE_INFO_BUFFER : REGISTER_CBV_ENEMY_CONE_INFO
{
    float4 worldPosAndDir[ENEMY_CONE_INFO_CAPACITY]; // XY is world pos in XZ plane and ZW is world direction in XZ plane.
    float coneLength;
    float coneAngle;
    uint coneCount;
    
    float MOREPADDING;
}

float3 DrawEnemyCone(float3 color, float3 enemyPos, float3 worldPos, float3 viewDirection, float coneLength, float coneAngle)
{
    float3 enemyToPoint = worldPos - enemyPos;
    float dirAlignment = dot(normalize(enemyToPoint), normalize(viewDirection));
    float isInCone = step(cos(coneAngle), dirAlignment); // If aligned within cone angle.
    float coneInfluence = 1.0f - smoothstep(0.0f, coneLength * 1.2f, length(enemyToPoint));
    
    float maxHeight = 2.0f;
    float heightInfluence = 1.0f - smoothstep(0.0f, maxHeight, worldPos.y);
	
    float totalInfluence = coneInfluence * isInCone * heightInfluence;
    
    float detectionScaling = 1.2f; // Makes detection reach max faster
    float detectionInfluence = detectionLevel * detectionScaling;
    float minStrength = 1.0f;
    float maxStrength = 7.0f;
    float colorStrength = lerp(minStrength, maxStrength, detectionInfluence);
    float3 baseConeColor = float3(0.5f, 0.2f, 0.0f);
    float3 detectedConeColor = float3(1.0f, 0.0f, 0.0f);
    float3 coneColor = lerp(baseConeColor, detectedConeColor, detectionInfluence) * colorStrength;
    
    //return lerp(color, coneColor, coneInfluence * isInCone * heightInfluence);
    return coneColor * totalInfluence;
}

Texture2D<float4> positionalTexture : REGISTER_SRV_TEX_DEFAULT;
Texture2D<float4> renderTextureCopy : REGISTER_SRV_COPY_SOURCE;
RWTexture2D<float4> renderTexture : REGISTER_UAV_RENDER_TARGET;

[numthreads(NUM_THREADS.x, NUM_THREADS.y, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    const uint3 texPos = uint3(DTid.xy, 0u);
    const float4 worldPos = positionalTexture.Load(texPos);
    float4 color = renderTextureCopy.Load(texPos);

	{
        const int posArrSize = 2;
        float3 posArr[posArrSize] = { float3(1.0f, 0.0f, 0.0f), float3(4.0f, 0.0f, 2.0f) };
	    
        // Will not loop if the state is in the future.
        for (uint i = 0; i < coneCount; i++)
        {
            float4 enemyPosAndDir = worldPosAndDir[i];
            
            color.rgb += DrawEnemyCone(
                color.rgb,
                float3(enemyPosAndDir.x, 0.0f, enemyPosAndDir.y),
                worldPos.xyz, // Y value is important so dont set to 0.
                float3(enemyPosAndDir.z, 0.0f, enemyPosAndDir.w),
                coneLength,
                coneAngle
            ) * !isInFuture;
            
            float dist = distance(enemyPosAndDir.xy, float2(worldPos.x, worldPos.z));
            float distInfluence = 1.0f - smoothstep(0.0f, 0.3f, dist);
            color.rgb += float3(2.0f, 0.0f, 0.0f) * distInfluence * isInFuture;
        }
    }
    
    renderTexture[texPos.xy] = color;
}