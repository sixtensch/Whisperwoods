#pragma once
#include "GameObject.h"

struct Light : GameObject // Inherits a transform.
{
	struct LightBufferData // Data structure base 
	{
		Vec3 position;
		Vec3 direction;
		Vec3 color;
	};
	Vec3 color;
	float intensity;
	void FillBasicBufferData(LightBufferData* outData, Transform* inTransform);
};

struct PointLight : Light
{
	struct PointLightBufferData : LightBufferData // Data structure to be stored in the structured buffer for point lights { vec3 vec3 vec3 vec3 } 3*4*4 = 48
	{
		Vec3 intensity; // In practice only uses x value.
	};
	PointLightBufferData bufferData;
	float range;
	void Update();
};

struct SpotLight : Light
{
	struct SpotLightBufferData : LightBufferData // Data structure to be stored in the structured buffer for spot lights { vec3 vec3 vec3 vec3 } 3*4*4 = 48
	{
		Vec3 intensityInnerOuter; // (intensity, FOVInner, FOVOuter)
	};
	SpotLightBufferData bufferData;
	float fovOuter;
	float fovInner;
	void Update();
};

struct DirectionalLight : Light
{
	struct DirectionalLightBufferData : LightBufferData // Data structure to be stored in the structured buffer for directional lights { vec3 vec3 vec3 vec3 } 3*4*4 = 48
	{
		Vec3 intensity; // Mostly padding as with point.
	};
	DirectionalLightBufferData bufferData;
	void Update();
};