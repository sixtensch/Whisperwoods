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

	struct PlayerAndWorldInfo
	{
		Mat4 worldMatrix;
		Vec4 worldInfo1;
		Vec4 worldInfo2;
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
		Vec3 fogFocus;
		float fogRadius;
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
		Vec3 ambient;
		float tiling;
	};


	struct GUIInfo
	{
		// Do not reorder
		Vec3 color;
		float alpha;
		Vec3 vectorData;
		float floatData;
		Point4 intData;
		Vec2 minimapRoomPos;
		
		Vec2 PADDING;
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

	struct GameLogicInfo
	{
		float timeSinceSwitch;
		float chargeDuration;
		float falloffDuration;
		float isInFuture; // Bool is 4 bytes on the GPU (hlsl) so no loss.

		float detectionLevel;
		float PADDING[3];
	};

	struct EnemyConeInfo
	{
		Vec4 worldPosAndDir[ENEMY_CONE_INFO_CAPACITY]; // XY is world pos in XZ plane and ZW is world direction in XZ plane.
		float coneLength; // Length of the vision cone for all enemies.
		float coneAngle; // Angle of the cone (whole cone) in radians for all enemies.
		uint coneCount;

		float PADDING;
	};
}

struct ConstantBuffers
{
	ComPtr<ID3D11Buffer> viewInfo;
	ComPtr<ID3D11Buffer> objectInfo;
	ComPtr<ID3D11Buffer> playerInfo;
	
	ComPtr<ID3D11Buffer> shadingInfo;
	ComPtr<ID3D11Buffer> materialInfo;
	ComPtr<ID3D11Buffer> guiInfo;

	ComPtr<ID3D11Buffer> ppfxThresholdInfo;
	ComPtr<ID3D11Buffer> ppfxColorGradeInfo;

	ComPtr<ID3D11Buffer> gameLogicInfo;

	ComPtr<ID3D11Buffer> enemyConeInfo;
};