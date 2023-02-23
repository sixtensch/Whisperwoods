#include "Constants.hlsli"
#include "ComputeConstants.hlsli"

float3 DrawEnemyCone(float3 color, float3 enemyPos, float3 worldPos, float3 viewDirection, float coneLength, float coneAngle)
{
    float3 enemyToPoint = worldPos - enemyPos;
    float angle = dot(normalize(enemyToPoint), normalize(viewDirection));
    float isInCone = step(cos(coneAngle), angle);
    float coneInfluence = 1.0f - smoothstep(0.0f, coneLength * 1.2f, length(enemyToPoint));
    
    float minHeight = 100.0f;
    float heightInfluence = step(worldPos.y, minHeight);
	
    float totalInfluence = coneInfluence * isInCone * heightInfluence;
    float3 coneColor = float3(5.0f, 0.4f, 0.0f);
	
    //return lerp(color, coneColor, coneInfluence * isInCone * heightInfluence);
    return coneColor * totalInfluence;
}

float3 ApplyExpFog(float3 color, float density, float distance, float3 fogColor, float fogStrength)
{
    float fogFactor = 1.0f / exp(pow(distance * density, 2.0f));
    fogFactor = 1.0f - fogFactor;
    
    return lerp(color, fogColor * fogStrength, fogFactor);
}


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
    bool isInFuture;
    
    bool PADDING[3];
}

cbuffer ENEMY_CONE_INFO_BUFFER : REGISTER_CBV_ENEMY_CONE_INFO
{
    float4 worldPosAndDir[ENEMY_CONE_INFO_CAPACITY]; // XY is world pos in XZ plane and ZW is world direction in XZ plane.
    float coneLength;
    float coneAngle;
    uint coneCount;
    
    float MOREPADDING;
}

Texture2D<float4> positionalTexture : REGISTER_SRV_TEX_DEFAULT;
Texture2D<float4> renderTextureCopy : REGISTER_SRV_COPY_SOURCE;
RWTexture2D<float4> renderTexture : REGISTER_UAV_RENDER_TARGET;

[numthreads(COMPUTE_THREADS_X, COMPUTE_THREADS_Y, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    const uint3 texPos = uint3(DTid.xy, 0u);
    const float4 worldPos = positionalTexture.Load(texPos);
    float4 color = renderTextureCopy.Load(texPos);

	{
        int posArrSize = 2;
        float3 posArr[] = { float3(1.0f, 0.0f, 0.0f), float3(4.0f, 0.0f, 2.0f) };
	    
        // Will not loop if the state is in the future.
        for (uint i = 0; i < coneCount * !isInFuture; i++)
        {
            float4 enemyPosAndDir = worldPosAndDir[i];
            
            color.rgb += DrawEnemyCone(
                color.rgb,
                float3(enemyPosAndDir.x, 0.0f, enemyPosAndDir.y),
                worldPos.xyz, // Y value is important so dont set to 0.
                float3(enemyPosAndDir.z, 0.0f, enemyPosAndDir.w),
                coneLength,
                coneAngle
            );
        }
    }
    
    float posToCamDist = distance(worldPos.xyz, cameraPosition);
    float3 fogColor[2] = { float3(0.8f.rrr), float3(1.2f, 0.8f, 0.3f) };
    float fogStrength[2] = { 1.0f, 1.7f };
    float fogDensity[2] = { 0.06f, 0.1f };
    uint stateIndex = uint(isInFuture);
    color.rgb = ApplyExpFog(color.rgb, fogDensity[stateIndex], posToCamDist, fogColor[stateIndex], fogStrength[stateIndex]);
    
    renderTexture[texPos.xy] = color;
}