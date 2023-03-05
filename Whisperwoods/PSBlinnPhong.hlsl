
#include "Constants.hlsli"
#include "PhongAlg.hlsli"

static float smoothing = 2.0f;

struct VSOutput
{
	float4 outPosition	: SV_POSITION;
    float4 wPosition    : WPOSITION0;
	float3 outNormal	: NORMAL0;
	float3 outTangent   : TANGENT0;
	float3 outBitangent : BITANGENT0;
	float2 outUV		: TEXCOORD0;
};

struct LightDirectional
{
	matrix clip;
    float3 direction;		float pad1;
	float3 intensity;		float pad2;
};

struct LightPoint
{
	float3 position;
	float range;
	float3 intensity;		float pad1;
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
	LightDirectional	directionalLight;
	LightPoint			pointLights[LIGHT_CAPACITY_POINT];
	LightSpot			spotLights[LIGHT_CAPACITY_SPOT];
	
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

float2 texOffset( int u, int v, int lNo )
{
	return float2(u * 1.0f / 2048, v * 1.0f /2048);
}

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


SamplerState textureSampler : REGISTER_SAMPLER_STANDARD;
SamplerComparisonState shadowSampler : REGISTER_SAMPLER_SHADOW;

Texture2D textureDiffuse : REGISTER_SRV_TEX_DIFFUSE;
Texture2D textureSpecular : REGISTER_SRV_TEX_SPECULAR;
Texture2D textureEmissive : REGISTER_SRV_TEX_EMISSIVE;
Texture2D textureNormal : REGISTER_SRV_TEX_NORMAL;
Texture2D shadowTexture : REGISTER_SRV_SHADOW_DEPTH;

struct PS_OUTPUT
{
    float4 MainTarget : SV_TARGET0;
    float4 PositionTarget : SV_TARGET1;
};

PS_OUTPUT main(VSOutput input)
{
	// Output struct for both main texture target and positional target.
    PS_OUTPUT output;
	
    float2 uv = input.outUV * tiling;
	
    float4 diffuseSample = textureDiffuse.Sample(textureSampler, uv);
	
    if (diffuseSample.a < 0.1f)
        discard;
	
    float4 specularSample = textureSpecular.Sample(textureSampler, uv);
    float4 emissiveSample = textureEmissive.Sample(textureSampler, uv);
    float4 normalSample = textureNormal.Sample(textureSampler, uv);
    normalSample.g = 1.0f - normalSample.g;
	
    float4 colorAlbedoOpacity = float4(diffuse * diffuseSample.xyz, alpha * diffuseSample.a);
    float4 colorSpecularSpecularity = float4(specular, glossiness) * specularSample;
    float3 colorEmissive = emissive * emissiveSample.xyz;
	
    float3 cameraDirection = normalize(cameraPosition - input.wPosition.xyz);
	
    float3x3 texSpace = float3x3(input.outTangent, input.outBitangent, input.outNormal);
    float3 normal = normalize(mul(2.0f * normalSample.xyz - float3(1.0f, 1.0f, 1.0f), texSpace));
	
	// Cumulative color
    float4 color = float4(colorAlbedoOpacity.xyz * ambient, colorAlbedoOpacity.w);
	
	// Check shadow
    float4 lsPos = mul(input.wPosition, directionalLight.clip);
    float4 lsNDC = lsPos / lsPos.w; // U, V, Depth
    float2 lsUV = float2(lsNDC.x * 0.5f + 0.5f, lsNDC.y * -0.5f + 0.5f);
	
    float dirNDotL = dot(normal, directionalLight.direction);
    float epsilon = 0.00005 / acos(saturate(dirNDotL));
    //bool shadowAff = shadowTexture.SampleCmp(shadowSampler, lsUV, lsNDC.z + epsilon).x;
	
    float sum = 0;
    float x, y;

	// PCF filtering (Smooth shadows)
	[unroll]
    for (y = -smoothing; y <= smoothing; y += 1.0f)
    {
		[unroll]
        for (x = -smoothing; x <= smoothing; x += 1.0f)
        {
            sum += shadowTexture.SampleCmpLevelZero(shadowSampler,
				lsUV.xy + texOffset(x, y, 0), lsNDC.z - epsilon);
        }
    }
    float shadowAff = sum / ((smoothing + smoothing + 1.0f) * (smoothing + smoothing + 1.0f));




    // Directional lighting
    color += shadowAff * phong(
		input.wPosition.xyz,
		normal,
		directionalLight.intensity,
		-directionalLight.direction,
		1.0f,
		cameraDirection,
		colorAlbedoOpacity.xyz,
		colorSpecularSpecularity.xyz,
		colorAlbedoOpacity.w,
		colorSpecularSpecularity.w
	);

	
    for (uint i = 0; i < pointCount; i++)
    {
        float3 lightVector = pointLights[i].position - input.wPosition.xyz;
        float lightDistanceInv = 1.0f / length(lightVector);
		
        color += phong(
			input.wPosition.xyz,
			normal,
			pointLights[i].intensity,
			lightVector * lightDistanceInv,
			lightDistanceInv * lightDistanceInv,
			cameraDirection,
			colorAlbedoOpacity.xyz,
			colorSpecularSpecularity.xyz,
			colorAlbedoOpacity.w,
			colorSpecularSpecularity.w
		);
    }
	
    for (uint j = 0; j < spotCount; j++)
    {
        float3 lightVector = spotLights[j].position - input.wPosition.xyz;
        float lightDistanceInv = 1.0f / length(lightVector);
        float3 lightDirection = lightVector * lightDistanceInv;
		
        float spotFadeWidth = (spotLights[j].cosInner - spotLights[j].cosOuter);
		
        float x = dot(lightDirection, -spotLights[j].direction);
        float k = 1.0f / (spotLights[j].cosInner - spotLights[j].cosOuter);
        float spotScalar = saturate(k * (x - spotLights[j].cosOuter));
		
        color += phong(
			input.wPosition.xyz,
			normal,
			spotLights[j].intensity,
			lightDirection,
			lightDistanceInv * lightDistanceInv * spotScalar,
			cameraDirection,
			colorAlbedoOpacity.xyz,
			colorSpecularSpecularity.xyz,
			colorAlbedoOpacity.w,
			colorSpecularSpecularity.w
		);
    }
	
	
    float3 finalEmissiveColor = 0.0f;
	// Detection scaled emission calculations.
	{
        float emissiveBrightness = length(colorEmissive);
	
        float detectionColorStrength = 2.0f * emissiveBrightness;
        float3 detectionColor = normalize(float3(2.0f, 0.2f, 0.0f)) * detectionColorStrength;
        detectionColor *= ceil(colorEmissive); // If the sample is 0, dont affect color. 
		
        float detectionBaseInfluence = 1.0f - emissiveSample.a;
        float detectionLevelScaling = 1.5f; // Makes detection level reach max faster.
        float detectionInfluence = smoothstep(0.0f, 1.0f, detectionLevelGlobal * detectionLevelScaling);
        float totalInfluence = detectionBaseInfluence * detectionInfluence;
		
        finalEmissiveColor = lerp(colorEmissive, detectionColor, totalInfluence);
    }
    
	
	// Used to scale ALL emissive for more dramatic glow.
    float emissiveScalar = 2.0f;
    color.rgb += finalEmissiveColor * emissiveScalar;
    color.a = saturate(color.a);
	
	
    output.MainTarget = color;
    output.PositionTarget = input.wPosition;
	
    return output;
}