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

	struct PPFXThresholdInfo 
	{
		float luminanceThreshold;
		float strength;
		float minLuminance;

		float PADDING;
	};

	struct PPFXColorGradeInfo
	{
		Vec2 vignetteBorderAndStrength;
		Vec2 contrastAmountAndMidpoint;
		float brightness;
		float saturation;

		Vec2 PADDING;
	};
}

struct ConstantBuffers
{
	ComPtr<ID3D11Buffer> viewInfo;
	ComPtr<ID3D11Buffer> objectInfo;
	
	ComPtr<ID3D11Buffer> shadingInfo;
	ComPtr<ID3D11Buffer> materialInfo;

	ComPtr<ID3D11Buffer> ppfxThresholdInfo;
	ComPtr<ID3D11Buffer> ppfxColorGradeInfo;
	
};