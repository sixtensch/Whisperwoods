#pragma once
#include "Light.h"

enum LightType
{
	LIGHT_TYPE_DIRECTIONAL = 0,
	LIGHT_TYPE_SPOTLIGHT = 1,
	LIGHT_TYPE_POINTLIGHT = 2
};
constexpr int START_MAX_LIGHTS = 40;

//class PointLight
//{};
//class DirectionalLight
//{};
//class SpotLight
//{};

class LightHandler sealed
{
public:
	LightHandler() = delete;
	LightHandler( ID3D11Device* device );
	~LightHandler();


	ID3D11Buffer* GetStructuredBufferP( LightType type );
	ID3D11Buffer* const* GetStructuredBufferPP( LightType type );

	
	void SetDirectionalLights( ID3D11DeviceContext* immediateContext, std::vector<DirectionalLight::DirectionalLightBufferData> data );
	void SetSpotLights( ID3D11DeviceContext* immediateContext, std::vector<SpotLight::SpotLightBufferData> data );
	void SetPointLights( ID3D11DeviceContext* immediateContext, std::vector<PointLight::PointLightBufferData> data );

	bool ReallocLights( ID3D11Device* device, LightType type );

private:
	bool InitPointSBuffer( ID3D11Device* device, UINT LightCount );
	bool InitDirSBuffer( ID3D11Device* device, UINT LightCount );
	bool InitSpotSBuffer( ID3D11Device* device, UINT LightCount );


public:
private:
	wrl::ComPtr<ID3D11Buffer> m_pointSBuffer;
	wrl::ComPtr<ID3D11Buffer> m_spotSBuffer;
	wrl::ComPtr<ID3D11Buffer> m_dirSBuffer;

	int m_maxSize[3];
};

