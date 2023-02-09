#pragma once
#include "GameObject.h"
#include "Camera.h"

enum LightType
{
	LightTypeDirectional = 0,
	LightTypeSpotlight = 1,
	LightTypePointlight = 2
};

struct Light : GameObject // Inherits a transform.
{
	cs::Color3f color;
	float intensity;
};

struct DirectionalLight : Light
{
	struct Data // Data structure to be stored in the structured buffer for directional lights { vec3 vec3 vec3 vec3 } 3*4*4 = 48
	{
		Mat4 clipMatrix;
		Vec3 direction;		PAD(4, 0);
		Vec3 intensity; 	PAD(4, 1);
	};

	// Member data
	float diameter;

	// Processed data
	Camera camera;
	Data bufferData;

	void Update();
};

struct PointLight : Light
{
	struct Data// Data structure to be stored in the structured buffer for point lights { vec3 vec3 vec3 vec3 } 3*4*4 = 48
	{
		Vec3 position;
		float range;
		Vec3 intensity;		PAD(4, 0);
	};

	// Member data
	float range;

	// Processed data
	Data bufferData;

	void Update();
};

struct SpotLight : Light
{
	struct Data // Data structure to be stored in the structured buffer for spot lights { vec3 vec3 vec3 vec3 } 3*4*4 = 48
	{
		Mat4 clipMatrix;
		Vec3 direction;
		float range;
		Vec3 position;
		float cosInner;
		Vec3 intensity;
		float cosOuter;
	};

	// Member data
	float fovOuter;
	float fovInner;
	float range;

	// Processed data
	Camera camera;
	Data bufferData;

	void Update();
};