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

	struct ShadingInfo
	{
		DirectionalLight::Data directional;
		PointLight::Data points[LIGHT_CAPACITY_POINT];
		SpotLight::Data spots[LIGHT_CAPACITY_SPOT];

		Vec3 ambientIntensity;
		uint pointCount;
		Vec3 cameraPosition;
		uint spotCount;
	};

	struct MaterialInfo
	{
		// Do not reorder
		Vec3 diffuse;
		float alpha;
		Vec3 specular;
		float glossiness;
		Vec3 emissive;
		float height;
		Vec3 pad;
		float tiling;
	};
}

struct ConstantBuffers
{
	ComPtr<ID3D11Buffer> viewInfo;
	ComPtr<ID3D11Buffer> objectInfo;
	ComPtr<ID3D11Buffer> playerInfo;
	
	ComPtr<ID3D11Buffer> shadingInfo;
	ComPtr<ID3D11Buffer> materialInfo;
};