
#include "Constants.hlsli"

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
	LightDirectional DirectionalLight;
	LightPoint PointLights[LIGHT_CAPACITY_POINT];
	LightSpot SpotLights[LIGHT_CAPACITY_SPOT];
	
	float3 ambient;
    uint spotCount;
    float3 cameraPosition;
    uint pointCount;
};


float4 main(VSOutput input) : SV_TARGET
{
    float magnitude = dot(input.outNormal, -DirectionalLight.direction);
    return float4(DirectionalLight.intensity * magnitude, 1.0f);
	
	//float3 normalColor = input.outNormal;
	//float3x3 TBN = float3x3(input.outTangent, input.outBitangent, input.outNormal);
	//// make normal in range -1 to 1
	//normalColor = (2.0f * normalColor) - 1.0f;
	//return  float4(mul(normalColor, TBN), 1.0);
	////return float4(input.outTangent,1);
}