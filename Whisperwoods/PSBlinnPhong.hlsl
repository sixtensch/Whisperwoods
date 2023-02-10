
#include "Constants.hlsli"
#include "PhongAlg.hlsli"

struct VSOutput
{
	float4 outPosition	: SV_POSITION;
    float4 wPosition    : WPOSITION0;
	float4 wsPosition	: WSPOSITION0;
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


float4 main(VSOutput input) : SV_TARGET
{
    float4 colorAlbedoOpacity = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 colorSpecularSpecularity = float4(0.6f, 0.6f, 0.6f, 20.0f);
    float3 colorEmissive = float3(0.0f, 0.0f, 0.0f);
	
    float3 cameraDirection = normalize(cameraPosition - input.wPosition.xyz);
	
	// Cumulative color
    float4 color = float4(colorAlbedoOpacity.xyz * ambient, colorAlbedoOpacity.w);
	
    color += phong(
		input.wPosition.xyz,
		input.outNormal,
		directionalLight.intensity,
		-directionalLight.direction,
		1.0f,
		cameraDirection,
		colorAlbedoOpacity.xyz,
		colorSpecularSpecularity.xyz,
		colorEmissive,
		colorAlbedoOpacity.w,
		colorSpecularSpecularity.w
	);
	
    for (uint i = 0; i < pointCount; i++)
    {
        float3 lightVector = pointLights[i].position - input.wPosition.xyz;
        float lightDistanceInv = 1.0f / length(lightVector);
		
        color += phong(
			input.wPosition.xyz,
			input.outNormal,
			pointLights[i].intensity,
			lightVector * lightDistanceInv,
			lightDistanceInv * lightDistanceInv,
			cameraDirection,
			colorAlbedoOpacity.xyz,
			colorSpecularSpecularity.xyz,
			colorEmissive,
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
			input.outNormal,
			spotLights[j].intensity,
			lightDirection,
			lightDistanceInv * lightDistanceInv * spotScalar,
			cameraDirection,
			colorAlbedoOpacity.xyz,
			colorSpecularSpecularity.xyz,
			colorEmissive,
			colorAlbedoOpacity.w,
			colorSpecularSpecularity.w
		);
    }
	
    return saturate(color);
}