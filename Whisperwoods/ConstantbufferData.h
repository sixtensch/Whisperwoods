#pragma once
#include "BindInfo.h"
#include "Light.h"

namespace CB
{
	struct ViewInfo
	{
		Mat4 viewMatrix;
		Mat4 projectionMatrix;
	};

	struct ObjectInfo
	{
		Mat4 worldMatrix;
	};

	//...
	struct ShadingInfo
	{
		DirectionalLight::Data directional;
		PointLight::Data points[LIGHT_CAPACITY_POINT];
		SpotLight::Data spots[LIGHT_CAPACITY_SPOT];

		Vec3 ambientIntensity;
		uint spotCount;
		Vec3 cameraPosition;
		uint pointCount;
	};
	// etc
}

struct ConstantBuffers
{
	ComPtr<ID3D11Buffer> viewInfo;
	ComPtr<ID3D11Buffer> objectInfo;
	
	ComPtr<ID3D11Buffer> shadingInfo;
};