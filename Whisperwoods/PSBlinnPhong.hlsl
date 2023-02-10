
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



cbuffer MaterialInfo : REGISTER_CBV_MATERIAL_INFO
{
    float3 diffuse;
    float alpha;
    float3 specular;
    float glossiness;
    float3 emissive;
    float height;
};

SamplerState textureSampler : REGISTER_SAMPLER_STANDARD;

Texture2D textureDiffuse : REGISTER_SRV_TEX_DIFFUSE;
Texture2D textureSpecular : REGISTER_SRV_TEX_SPECULAR;
Texture2D textureEmissive : REGISTER_SRV_TEX_EMISSIVE;
Texture2D textureNormal : REGISTER_SRV_TEX_NORMAL;



float4 main(VSOutput input) : SV_TARGET
{
    float4 diffuseSample = textureDiffuse.Sample(textureSampler, input.outUV);
    float4 specularSample = textureSpecular.Sample(textureSampler, input.outUV);
    float4 emissiveSample = textureEmissive.Sample(textureSampler, input.outUV);
    float4 normalSample = textureNormal.Sample(textureSampler, input.outUV);
	
    float4 colorAlbedoOpacity = float4(diffuse * diffuseSample.xyz, alpha * diffuseSample.a);
    float4 colorSpecularSpecularity = float4(specular, glossiness) * specularSample;
    float3 colorEmissive = emissive * emissiveSample.xyz;
	
    float3 cameraDirection = normalize(cameraPosition - input.wPosition.xyz);
	
    float3x3 texSpace = float3x3(input.outTangent, input.outBitangent, input.outNormal);
    float3 normal = normalize(mul(2.0f * normalSample.xyz - float3(1.0f, 1.0f, 1.0f), texSpace));
	
	// Cumulative color
    float4 color = float4(colorAlbedoOpacity.xyz * ambient, colorAlbedoOpacity.w); 
	
    color += phong(
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
	
    color += float4(colorEmissive.xyz, 0.0f);
	
    return saturate(color );
}