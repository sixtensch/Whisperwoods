#pragma once

// Sampler and Sampler State registers
enum RegSampler : char
{
	RegSamplerDefault = 0,

	RegSamplerUser0 = 1,
	RegSamplerUser1 = 2,
	RegSamplerUser2 = 3,
	RegSamplerUser3 = 4,
	RegSamplerUser4 = 5,

	RegSamplerSystem0 = 6,
	RegSamplerSystem1 = 7,
	RegSamplerSystem2 = 8,
	RegSamplerSystem3 = 9,

	RegSamplerStandard = 10,
	RegSamplerPoint = 11,
	RegSamplerShadow = 12,

	RegSamplerCount = 16	// <-- Keep last!
};

// Constant Buffer View registers
enum RegCBV : char
{
	RegCBVDefault = 0,

	RegCBVThresholdInfo = 1, // Lum threshold, strength, min lum
	RegCBVColorgradeInfo = 2, // Vignette, contrast, britghness, saturation
	RegCBVTimeSwitchInfo = 3, // Time since switch, start dur, end dur
	RegCBVEnemyConeInfo = 4, // 

	RegCBVGUIInfo = 5,
	RegCBVSystem1 = 6,
	RegCBVSystem2 = 7,

	RegCBVTesselationInfo = 8,
	RegCBVViewInfo = 9,			// View matrix, projection matrix, etc
	RegCBVObjectInfo = 10,		// World matrix, etc
	RegCBVMaterialInfo = 11,
	RegCBVClientInfo = 12,		// Screen size, etc
	RegCBVShadingInfo = 13,

	RegCBVCount = 14	// <-- Keep last!
};

// Unordered Access View registers
enum RegUAV : char
{
	RegUAVDefault = 0,

	RegUAVUser0 = 1,
	RegUAVUser1 = 2,
	RegUAVUser2 = 3,

	RegUAVSystem0 = 4,
	RegUAVSystem1 = 5,

	RegUAVRenderTarget = 6,

	RegUAVCount = 8		// <-- Keep last!
};

// Shader Resource View registers
enum RegSRV : char
{
	RegSRVDefault = 0,

	RegSRVUser0 = 1,
	RegSRVUser1 = 2,
	RegSRVUser2 = 3,
	RegSRVUser3 = 4,
	RegSRVUser4 = 5,
	RegSRVUser5 = 6,

	RegSRVTexDiffuse = 7,
	RegSRVTexSpecular = 8,
	RegSRVTexEmissive = 9,
	RegSRVTexNormal = 10,

	RegSRVLightsDirectional = 11,
	RegSRVLightsPoint = 12,
	RegSRVLightsSpot = 13,

	RegSRVCopySource = 20,
	RegSRVParticleData = 21,

	RegSRVCubemap = 22,
	RegSRVShadowDepth = 24, // Array of up to FOX_C_MAX_SHADOWS shadowed lights

	RegSRVCount = 24 + LIGHT_CAPACITY_DIR + LIGHT_CAPACITY_SPOT	// <-- Keep last!
};