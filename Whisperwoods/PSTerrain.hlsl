
#include "Constants.hlsli"
#include "PhongAlgLite.hlsli"
#include "FogFuncs.hlsli"
#include "TimeSwitchFuncs.hlsli"
#include "BrightnessFilteringIncludes.hlsli"
#include "EnemyConeVisIncludes.hlsli"

static float smoothing = 2.0f;

struct VSOutput
{
    float4 outPosition : SV_POSITION;
    float4 wPosition : WPOSITION0;
    float3 outNormal : NORMAL0;
    float3 outTangent : TANGENT0;
    float3 outBitangent : BITANGENT0;
    float4 outUV : TEXCOORD0;
};

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

// Used here for getting detection rate.
cbuffer TIME_SWITCH_INFO_BUFFER : REGISTER_CBV_SWITCH_INFO
{
    float timeSinceSwitch;
    float timeSwitchStartDuration;
    float timeSwitchEndDuration;
    float isInFuture; // Bool is 4 bytes.
    
    float detectionLevelGlobal;
    float PADDING[3];
}

cbuffer THRESHOLD_INFO_BUFFER : REGISTER_CBV_THRESHOLD_INFO
{
    float luminanceThreshold;
    float strength;
    float minLuminance;
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

cbuffer MaterialInfo : REGISTER_CBV_MATERIAL_INFO
{
    float3 diffuse;
    float alpha;
    float3 specular;
    float glossiness;
    float3 emissive;
    float height;
    float3 pad;
    float tiling;
};

float2 texOffset(int u, int v, int lNo)
{
    return float2(u * 1.0f / 2048, v * 1.0f / 2048);
}

cbuffer ENEMY_CONE_INFO_BUFFER : REGISTER_CBV_ENEMY_CONE_INFO
{
    float4 worldPosAndDir[ENEMY_CONE_INFO_CAPACITY]; // XY is world pos in XZ plane and ZW is world direction in XZ plane.
    float coneLength;
    float coneAngle;
    uint coneCount;
    
    float MOREPADDING;
}


SamplerState textureSampler : REGISTER_SAMPLER_STANDARD;
SamplerComparisonState shadowSampler : REGISTER_SAMPLER_SHADOW;

Texture2D textureBitmap : REGISTER_SRV_TEX_USER_5;

Texture2D textureDiffuse : REGISTER_SRV_TEX_DIFFUSE;
Texture2D textureSpecular : REGISTER_SRV_TEX_SPECULAR;
Texture2D textureEmissive : REGISTER_SRV_TEX_EMISSIVE;
Texture2D textureNormal : REGISTER_SRV_TEX_NORMAL;

Texture2D shadowTextureStatic : REGUSTER_SRV_SHADOW_STATIC;
Texture2D shadowTextureDynamic : REGISTER_SRV_SHADOW_DEPTH;

struct PS_OUTPUT
{
    float4 MainTarget : SV_TARGET0;
    float4 LuminanceTexture : SV_TARGET1;
};

PS_OUTPUT main(VSOutput input)
{
	// Output struct for both main texture target and positional target.
    PS_OUTPUT output;
    //output.MainTarget = float4(1.0f, 0.0f, 1.0f, 1.0f);
    //output.LuminanceTexture = 0.0f.rrrr;
    //return output;
    
    // Diffuse sample
    float2 uv = input.outUV.xy * tiling;  
    float2 uv2 = input.outUV.xy * tiling * 1.5f;
    float4 diffuseSample = textureDiffuse.Sample(textureSampler, uv);
    if (diffuseSample.a < 0.1f)
        discard;
	
    
    // Mix the color depending on if it's the background or the ground (UV.z value)
    float4 colorAlbedoOpacity;
    float4 colorAlbedoOpacity2;
    if (input.outUV.z > 0.0f)
    {
        // Compensate for the stretch
        float2 uvNoTile = input.outUV.xy;
        uvNoTile.x -= 0.5;
        uvNoTile.x *= 1.2;
        uvNoTile.x += 0.5;
        uvNoTile.y += 0.5;
        uvNoTile.y *= 1.2;
        uvNoTile.y -= 0.5;
        // Bitmap sample
        float4 bitmapSample = textureBitmap.Sample(textureSampler, uvNoTile);
        
        float difference = abs(bitmapSample.r - bitmapSample.b);
        float factor = bitmapSample.r * bitmapSample.b + (difference*0.9f);
        float blendFactor = max(factor, 0.1f);
        
        float4 diffuseSample2 = textureDiffuse.Sample(textureSampler, uv * (1.0f - (factor * 0.01f)));
        float4 diffuseSample3 = textureDiffuse.Sample(textureSampler, uv2); // Detail Texture
        colorAlbedoOpacity = float4(diffuse * diffuseSample2.xyz * blendFactor, alpha * diffuseSample.a);
        colorAlbedoOpacity2 = float4(diffuse * diffuseSample3.xyz * (1.0f-blendFactor)*1.1f, alpha * diffuseSample.a); // use the inverted factor for the dark areas
    }
    else
    {
        colorAlbedoOpacity = float4(diffuse * diffuseSample.xyz * min(pow(input.wPosition.y, 4), 1.0f), alpha * diffuseSample.a); 
    }
    
    // Normal
    float3 normal = input.outNormal;
    
	// Cumulative color
    float4 color = float4(colorAlbedoOpacity.xyz * ambient, colorAlbedoOpacity.w);
	
	// Check shadow
    float4 lsPos = mul(input.wPosition, directionalLight.clip);
    float4 lsNDC = lsPos / lsPos.w; // U, V, Depth
    float2 lsUV = float2(lsNDC.x * 0.5f + 0.5f, lsNDC.y * -0.5f + 0.5f);
    float dirNDotL = dot(normal, directionalLight.direction);
    float epsilon = 0.00005 / acos(saturate(dirNDotL));    
    
    // Distance based smoothing (comment out for minor peformance boost possibly)
    //float shadowSample = shadowTexture.Sample(textureSampler, lsUV).x;
    //float shadowDiff = (shadowSample * 10 - saturate((lsNDC.z - epsilon) * 10));
    //float3 color2 = abs(saturate(shadowDiff * shadowDiff * 20000.0f));
    //float3 shadowSmoothVal = max(saturate(color2), 0.01f);   
    //float kernelWidth = 0.5f + (shadowSmoothVal.r*2);
    float kernelWidth = 1.0f; // Comment in
    
	// PCF filtering (Smooth shadows)
    float sum = 0;
    float x, y;
    int indexer = 0; // Used for some randomness to the sampling
	[unroll]
    for (y = -smoothing; y <= smoothing; y += 1.0f)
    {
		[unroll]
        for (x = -smoothing; x <= smoothing; x += 1.0f)
        {   
            sum += min(
                shadowTextureStatic.SampleCmpLevelZero(shadowSampler,
				    lsUV.xy + texOffset(
                (x + (-1 + ((indexer + 1) % 2) * 2)) * kernelWidth, 
                (y + (-1 + (indexer % 2) * 2)) * kernelWidth, 0),
                lsNDC.z - epsilon),
            shadowTextureDynamic.SampleCmpLevelZero(shadowSampler,
				    lsUV.xy + texOffset(
                (x + (-1 + ((indexer + 1) % 2) * 2)) * kernelWidth,
                (y + (-1 + (indexer % 2) * 2)) * kernelWidth, 0),
                lsNDC.z - epsilon)
            );
            
            
            indexer++;
    
        }
    }
    // adjust
    float shadowAff = sum / 25.0f;
    
    // Simple lighting
    if (input.outUV.z > 0.0f)
    {
        color += shadowAff * phongLite(
		directionalLight.intensity,
		1.0f,
		colorAlbedoOpacity.xyz + (colorAlbedoOpacity2.xyz * 0.5f) // Combine the normal and detail samples
	);
    }
    else
    {
        color += shadowAff * phongLite(
		directionalLight.intensity,
		1.0f,
		colorAlbedoOpacity.xyz
        );

    }
    
    // Enemy cone effects.
    // Will not loop if the state is in the future.
    for (uint i = 0; i < coneCount * !isInFuture; i++)
    {
        float4 enemyPosAndDir = worldPosAndDir[i];
        float3 enemyPos = float3(enemyPosAndDir.x, 0.0f, enemyPosAndDir.y);
        float3 enemyDir = float3(enemyPosAndDir.z, 0.0f, enemyPosAndDir.w);
        
        color.rgb += DrawEnemyCone(
            enemyPos,
            input.wPosition.xyz, // Y value is important so dont set to 0.
            enemyDir,
            coneLength,
            coneAngle,
            detectionLevelGlobal
        );
    }
    
    // Enemy position effects.
    // Will not loop if the state is in the present.
    for (uint i = 0; i < coneCount * isInFuture; i++)
    {
        float4 enemyPosAndDir = worldPosAndDir[i];
        float3 enemyPos = float3(enemyPosAndDir.x, 0.0f, enemyPosAndDir.y);
        
        color.rgb += DrawEnemyPos(enemyPos, input.wPosition.xyz) * isInFuture;
    }
	
    // Fog effects
    {
        float posToCamDist = distance(input.wPosition.xyz, cameraPosition);
        uint stateIndex = uint(isInFuture);
        color.rgb = ApplyExpFog(
            color.rgb,
            STATE_FOG_DENSITIES[stateIndex],
            posToCamDist,
            STATE_FOG_COLORS[stateIndex],
            STATE_FOG_STRENGTHS[stateIndex]
        );
    }
	
    float totalInflunce =
        TotalTimeSwitchInfluence(
        timeSinceSwitch,
        timeSwitchStartDuration,
        timeSwitchEndDuration
    );
    
    float3 lumColor = FilterBrightness(
        color.rgb,
        lerp(luminanceThreshold, 0.0f, totalInflunce),
        lerp(strength, strength * 1.4f, totalInflunce),
        lerp(minLuminance, 0.0f, totalInflunce)
    );
    
    // Send the final color.
    color.a = saturate(color.a); 
    output.MainTarget = color;
    output.LuminanceTexture = float4(lumColor, color.a);
	
    return output;
}


// Old shader stuff


    //float4 specularSample = textureSpecular.Sample(textureSampler, uv);
    //float4 emissiveSample = textureEmissive.Sample(textureSampler, uv);
    //float4 normalSample = textureNormal.Sample(textureSampler, uv);
    //normalSample.g = 1.0f - normalSample.g;

    //float4 colorSpecularSpecularity = float4(specular, glossiness) * specularSample;
    //float3 colorEmissive = emissive * emissiveSample.xyz;
    //float3 cameraDirection = normalize(cameraPosition - input.wPosition.xyz);
    //float3x3 texSpace = float3x3(input.outTangent, input.outBitangent, input.outNormal);
    //float3 normal = normalize(mul(2.0f * normalSample.xyz - float3(1.0f, 1.0f, 1.0f), texSpace));


  //  for (uint i = 0; i < pointCount; i++)
  //  {
  //      float3 lightVector = pointLights[i].position - input.wPosition.xyz;
  //      float lightDistanceInv = 1.0f / length(lightVector);
		
  //      color += phong(
		//	input.wPosition.xyz,
		//	normal,
		//	pointLights[i].intensity,
		//	lightVector * lightDistanceInv,
		//	lightDistanceInv * lightDistanceInv,
		//	cameraDirection,
		//	colorAlbedoOpacity.xyz,
		//	colorSpecularSpecularity.xyz,
		//	colorAlbedoOpacity.w,
		//	colorSpecularSpecularity.w
		//);
  //  }
	
  //  for (uint j = 0; j < spotCount; j++)
  //  {
  //      float3 lightVector = spotLights[j].position - input.wPosition.xyz;
  //      float lightDistanceInv = 1.0f / length(lightVector);
  //      float3 lightDirection = lightVector * lightDistanceInv;
		
  //      float spotFadeWidth = (spotLights[j].cosInner - spotLights[j].cosOuter);
		
  //      float x = dot(lightDirection, -spotLights[j].direction);
  //      float k = 1.0f / (spotLights[j].cosInner - spotLights[j].cosOuter);
  //      float spotScalar = saturate(k * (x - spotLights[j].cosOuter));
		
  //      color += phong(
		//	input.wPosition.xyz,
		//	normal,
		//	spotLights[j].intensity,
		//	lightDirection,
		//	lightDistanceInv * lightDistanceInv * spotScalar,
		//	cameraDirection,
		//	colorAlbedoOpacity.xyz,
		//	colorSpecularSpecularity.xyz,
		//	colorAlbedoOpacity.w,
		//	colorSpecularSpecularity.w
		//);
  //  }
	
	
    //float3 finalEmissiveColor = 0.0f;
	//// Detection scaled emission calculations.
	//{
 //       float emissiveBrightness = length(colorEmissive);
	
 //       float detectionColorStrength = 2.0f * emissiveBrightness;
 //       float3 detectionColor = normalize(float3(2.0f, 0.2f, 0.0f)) * detectionColorStrength;
 //       detectionColor *= ceil(colorEmissive); // If the sample is 0, dont affect color. 
		
 //       float detectionBaseInfluence = 1.0f - emissiveSample.a;
 //       float detectionLevelScaling = 1.5f; // Makes detection level reach max faster.
 //       float detectionInfluence = smoothstep(0.0f, 1.0f, detectionLevelGlobal * detectionLevelScaling);
 //       float totalInfluence = detectionBaseInfluence * detectionInfluence;
		
 //       finalEmissiveColor = lerp(colorEmissive, detectionColor, totalInfluence);
 //   }


 //   // Directional lighting
 //   color += shadowAff * phongLite(
	//	//input.wPosition.xyz,
	//	//normal,
	//	directionalLight.intensity,
	//	//-directionalLight.direction,
	//	1.0f,
	//	//cameraDirection,
	//	colorAlbedoOpacity.xyz
	//	//colorSpecularSpecularity.xyz,
	//	//colorAlbedoOpacity.w,
	//	//colorSpecularSpecularity.w
	//);

	// Used to scale ALL emissive for more dramatic glow.
    //float emissiveScalar = 1.0f;
    //color.rgb += finalEmissiveColor * emissiveScalar;